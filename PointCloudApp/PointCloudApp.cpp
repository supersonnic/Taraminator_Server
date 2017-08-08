///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, e-Con Systems.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT/INDIRECT DAMAGES HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/**********************************************************************
PointCloudApp: Defines the methods to view the pointcloud image of the
			   scene.
**********************************************************************/

#include "PointCloudApp.h"

#include "../cpp_server_client/server.h"
#include "OctreeVoxelGridFilter.h"

//	GLOBAL VARIABLES

/**********************************************************************/

pcl::visualization::CloudViewer *g_PclViewer;//("Stereo Point Cloud Viewer");
Server server;

/**********************************************************************/





//Constructor
PointCloudApp::PointCloudApp(void)
{
	//init the variables
	Trans_x = 0.0;
	Trans_y = 0.0;
	Trans_z = 15.0;
	Rot_x = 0.0;
	Rot_y = 0.0;
	Rot_z = 0.0;
	m_MinDepth = 10000.0;
}

//Initialises all the necessary files
int PointCloudApp::Init()
{
	cout << "		Point Cloud " << endl << endl;
	cout << " 3D point cloud is rendered in the window!" << endl << endl;
	cout << " Scroll/Move/Zoom(In/Out) on the window to view the Point Cloud data.!" << endl ;
	cout << " Press 'Alt + R' on the window to view the initial data!" << endl << endl;

	//Initialise the camera
	if(!_Disparity.InitCamera(true, true))
	{
		if(DEBUG_ENABLED)
			cout << "Camera Initialisation Failed\n";
		return FALSE;
	}

	//Camera Streaming
	CameraStreaming();

	return TRUE;
}

/*
	Method name: getPC()
	function: allows to extract the point cloud data into a file with extension "pcd" (point cloud data)

/*/
void PointCloudApp::getPC(PointCloud<PointXYZRGB>::Ptr cloudptr)
{
/*	pcl::PointCloud<pcl::PointXYZ> cloud;

  // Fill in the cloud data
  cloud.width    = 756;
  cloud.height   = 480;
  cloud.is_dense = false;
  cloud.points.resize (cloud.width * cloud.height);

  for (size_t i = 0; i < cloud.points.size (); ++i)
  {
    cloud.points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
    cloud.points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
    cloud.points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);
  }
*/

  //cout << ("sending points to server...");
  PointCloud<PointXYZRGB> cloud = *cloudptr;


	// check to see if the cloud is empty or if the client hasn't connected
	if (!cloud.empty()){

		

		//pcl::io::savePCDFileASCII ("camera_pcd.pcd", cloud);
		//std::cerr << "Saved " << cloud.points.size () << " data points to camera_pcd.pcd." << std::endl;

		//server.serverTask(cloud);

		/*// print cloud
		for (size_t i = 0; i < cloud.points.size (); ++i)
		{
			// pass current point cloud to socket

			server.serverTask(cloud.points[i].x, cloud.points[i].y, cloud.points[i].z, cloud.points[i].rgb);		
			//server.serverTask(cloud);
			
			//std::cerr << "    " << cloud.points[i].x << " " << cloud.points[i].y << " " << cloud.points[i].z << std::endl;
		}*/

	}

}

//Streams the input from the camera
int PointCloudApp::CameraStreaming()
{
	char WaitKeyStatus;
	bool statusShowPointCloud = TRUE;
	int BrightnessVal = 4;		//Default value
	int ManualExposure = 0;

	//Window creation
	//namedWindow("Disparity Map", cv::WINDOW_AUTOSIZE);
	//namedWindow("Left Image", cv::WINDOW_AUTOSIZE);
	//namedWindow("Right Image", cv::WINDOW_AUTOSIZE);

	//g_PclViewer = new pcl::visualization::CloudViewer("3D Viewer");

	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//streaming
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftFrame, &RightFrame)) //Reads the frame and returns the rectified image
		{
			//delete g_PclViewer;
			cv::destroyAllWindows();
			break;
		}

		//Get disparity
		_Disparity.GetDisparity(LeftFrame, RightFrame, &gDisparityMap, &gDisparityMap_viz);

		//Pointcloud display
		statusShowPointCloud = ShowPointCloud();

		// instead of showing the point cloud on the 3dViewer, write the points to a file called "camera_pcd.pcd"

		
		//imshow("Disparity Map", gDisparityMap_viz);
		//imshow("Left Image", LeftFrame) ;
		//imshow("Right Image", RightFrame);

		//waits for the Key input
		WaitKeyStatus = cv::waitKey(1);

		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27 || statusShowPointCloud == FALSE)
		{
			//delete g_PclViewer;
			cv::destroyAllWindows(); //Closes all the windows
			break;
		}
		else if(WaitKeyStatus == 'b' || WaitKeyStatus == 'B') //Brightness
		{
			cout << endl << "Enter the Brightness Value, Range(1 to 7): " << endl;
			BrightnessVal = 0;
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats and Alphanumeric strings
			{
				std::stringstream ss(Inputline);
				if (ss >> BrightnessVal)
				{
					if (ss.eof())
					{
						//Setting up the brightness of the camera
						if (BrightnessVal >= 1  && BrightnessVal <= 7)
						{
							//Setting up the brightness
			                //In opencv-linux 3.1.0, the value needs to be normalized by max value (7)
			                _Disparity.SetBrightness((double)BrightnessVal / 7.0);
						}
						else
						{
							 cout << endl << " Value out of Range - Invalid!!" << endl;
						}
						break;
					}
				}
				BrightnessVal = -1;
				break;
			}

			if(BrightnessVal == -1)
			{
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}
		}
		//Sets up Auto Exposure
		else if(WaitKeyStatus == 'a' || WaitKeyStatus == 'A' ) //Auto Exposure
		{
			_Disparity.SetAutoExposure();
		}
		else if(WaitKeyStatus == 'e' || WaitKeyStatus == 'E') //Set Exposure
		{
			cout << endl << "Enter the Exposure Value Range(10 to 1000000 micro seconds): " << endl;

			ManualExposure = 0;
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats fg and Alphanumeric strings
			{
				std::stringstream ss(Inputline);
				if (ss >> ManualExposure)
				{
					if (ss.eof())
					{
						if(ManualExposure >= SEE3CAM_STEREO_EXPOSURE_MIN && ManualExposure <= SEE3CAM_STEREO_EXPOSURE_MAX)
						{
							//Setting up the exposure
							_Disparity.SetExposure(ManualExposure);
						}
						else
						{
							cout << endl << " Value out of Range - Invalid!!" << endl;
						}
						break;
					}
				}
				ManualExposure = -1;
				break;
			}

			if(ManualExposure == -1)
			{
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}
		}
	}

	return TRUE;
}

int PointCloudApp::ShowPointCloud()
{
	#pragma region For Point Cloud Rendering

		cv::Mat xyz;
        reprojectImageTo3D(gDisparityMap, xyz, _Disparity.DepthMap, true);

		PointCloud<PointXYZRGB>::Ptr point_cloud_ptr (new PointCloud<PointXYZRGB>);
		const double max_z = 1.0e4;
		
		int count = 0;

		for (int Row = 0; Row < xyz.rows; Row++)
		{
			for (int Col = 0; Col < xyz.cols-100; Col++)
			{
				PointXYZRGB point;
				uchar Pix=(uchar)255;
				//Just taking the Z Axis alone
				cv::Vec3f Depth = xyz.at<cv::Vec3f>(Row,Col);
				point.x = Depth[0];
				point.y = -Depth[1];
				point.z = -Depth[2];

				if(fabs(Depth[2] - max_z) < FLT_EPSILON || fabs(Depth[2]) > max_z|| Depth[2] < 0 || Depth[2] > m_MinDepth) 
					continue;

				Pix = LeftFrame.at<uchar>(Row, Col);
				uint32_t rgb = (static_cast<uint32_t>(Pix) << 16 |static_cast<uint32_t>(Pix) << 8 | static_cast<uint32_t>(Pix));

				point.rgb= *reinterpret_cast<float*>(&rgb);
								
				point_cloud_ptr->points.push_back (point);
//				cout << "cloud point: " << point.x << " " << point.y <<  " " << point.z << " "<< endl;				

//				server.serverTask(point);

			}

		}

		VoxelGrid<pcl::PointXYZRGB> vg;
		PointCloud<PointXYZRGB>::Ptr cloud_filtered(new PointCloud<PointXYZRGB>);



		vg.setInputCloud(point_cloud_ptr);
		vg.setLeafSize( .05f, .05f, .05f );
		vg.filter(*cloud_filtered);

		OctreeCentroidContainer octreeFilter;

		int conversion_factor = 1;
		for(int i; i < cloud_filtered->points.size(); i++)
		{
/*
			PointXYZRGB point_short;

			point_short.x = static_cast<short> (cloud_filtered->points[i].x * conversion_factor);
			point_short.y = static_cast<short> (cloud_filtered->points[i].y * conversion_factor);
			point_short.z = static_cast<short> (cloud_filtered->points[i].z * conversion_factor);
			point_short.rgb = cloud_filtered->points[i].rgb;
				
			//cout << "point_short: " << point_short.x << " " << point_short.y <<  " " << point_short.z << " "<< endl;
*/			
			server.serverTask(cloud_filtered->points[i]);
		}

		//cout << "size of point cloud: " << sizeof(point_cloud_ptr) <<endl;


/*
		point_cloud_ptr->width = (int) point_cloud_ptr->points.size();
		point_cloud_ptr->height = 1;

		Eigen::Affine3f Transform_Matrix = Eigen::Affine3f::Identity();

		// Define a translation of 2.5 meters on the x axis.
		Transform_Matrix.translation() << Trans_x, Trans_y, Trans_z;

		// The same rotation matrix as before; tetha radians arround Z axis
		Transform_Matrix.rotate (Eigen::AngleAxisf (Rot_x, Eigen::Vector3f::UnitX()));
		Transform_Matrix.rotate (Eigen::AngleAxisf (Rot_y, Eigen::Vector3f::UnitY()));
		Transform_Matrix.rotate (Eigen::AngleAxisf (Rot_z, Eigen::Vector3f::UnitZ()));

		PointCloud<PointXYZRGB>::Ptr point_cloud_Transformed_ptr (new PointCloud<PointXYZRGB>);	
		transformPointCloud (*point_cloud_ptr, *point_cloud_Transformed_ptr, Transform_Matrix);
*/


	// get transformed points from camera
		//getPC(point_cloud_Transformed_ptr);

		//PointCloud<PointXYZRGB> temp_cloud = *point_cloud_ptr;
		//server.serverTask(temp_cloud);


/*
		if(!g_PclViewer->wasStopped())
		{
			//g_PclViewer->showCloud(point_cloud_Transformed_ptr);
			g_PclViewer->showCloud(point_cloud_ptr);
		}
		else
		{
			printf ("\nQuitting PclViewer\n");
			return FALSE;
		}

*/
	#pragma endregion For Point Cloud Rendering

	return TRUE;

}

//Main Function
int main()
{
	if(DEBUG_ENABLED)
	{
		cout << "Point Cloud - Application\n";
		cout << "-------------------------\n\n";
	}


	// start server first and wait for client to connect
	server.startServer();

	//Object creation
	PointCloudApp _PointCloud;
	_PointCloud.Init();


	if(DEBUG_ENABLED)
		cout << "Exit - Point Cloud Application\n";
	return TRUE;
}
