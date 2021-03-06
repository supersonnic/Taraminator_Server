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
#pragma once
#include "Tara.h"
 
#include "boost/thread/thread.hpp"
#include "pcl/common/common_headers.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/visualization/cloud_viewer.h"
#include "pcl/common/transforms.h"
#include "pcl/io/pcd_io.h"
#include "pcl/point_types.h"
#include <pcl/common/time.h>

#include <pcl/filters/voxel_grid.h>
#include <pcl/io/io.h>
#include <pcl/octree/octree.h>

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT		480


using namespace Tara;
using namespace pcl;

class PointCloudApp
{
public:

	//Constructor
	PointCloudApp(void);

	//Initialise
	int Init();

private:
	
	//Mat to hold disparity
	cv::Mat gDisparityMap, gDisparityMap_viz;
	cv::Mat LeftFrame, RightFrame;

	//PCL Viz
	float Trans_x, Trans_y, Trans_z;
	float Rot_x, Rot_y, Rot_z;
	double m_MinDepth;
	
	//Display the Point Cloud
	int ShowPointCloud();

	//Camera Streaming
	int CameraStreaming();

	//Function for making PC file
	void getPC(PointCloud<PointXYZRGB>::Ptr cloudptr);

	//Object to access Disparity
	Disparity _Disparity;

};

