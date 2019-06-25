#include "mmwavebs.h"

mmWaveBS::mmWaveBS(double x, double y, uint32_t id, Status st)
: the_thread()
{
	m_xx =x; m_yy =y; m_id =id; m_status = st;
}

mmWaveBS::~mmWaveBS()
{
	stop_thread = true;
	if(the_thread.joinable()) 
		the_thread.join();
// 	std::cout << "Deconstructor " << __FILE__ << std::endl;
}


void mmWaveBS::Start()
{
    // This will start the thread. Notice move semantics!
    the_thread = std::thread(&mmWaveBS::ThreadMain,this);
    
    char thread_name [20];
    sprintf(thread_name, "BS_%d",m_id);
    prctl(PR_SET_NAME,thread_name,0,0,0);
    
    char thread_name_buffer[20];
    prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
    std::cout << "Class " << thread_name_buffer << "has started!" << std::endl;
}

void mmWaveBS::listen(const std::string& message)
{
    std::cout << " received: " << message << std::endl;
}

void mmWaveBS::ThreadMain()
{
    while(!stop_thread)
    {
		if(m_status == Status::idle)
		{
			srand(time(NULL));
			double p = ((double)rand()/(double)(RAND_MAX));
// 			std::cout << "pp = " << p << std::endl;
			counter(p * 10.0);
			if(m_status == Status::idle)
			{
				candidacy_msg message(m_xx, m_yy, m_id);
				candidacy.emit(message);
			}
		}

        // Do something useful, e.g:
//         std::this_thread::sleep_for( std::chrono::seconds(1) );
    }
}

void mmWaveBS::setColor(std::size_t color)
{
	m_color = color;
	std::size_t red = (color & 0xff0000) >> 16; m_rgb_color[0] = red;
	std::size_t green =(color & 0x00ff00) >> 8; m_rgb_color[1] = green;
	std::size_t blue = (color & 0x0000ff);  	m_rgb_color[2] = blue;
}

void mmWaveBS::declare_as_cluster_head()
{
	cluster_head_msg message(m_xx, m_yy, m_id, m_color);
	clusterHead.emit(message);
}
