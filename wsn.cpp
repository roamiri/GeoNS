
#include "wsn.h"

WSN::WSN(std::string svg_name):Container(svg_name)
{

}

WSN::~WSN()
{

}


void WSN::listen_For_Candidacy(const candidacy_msg& message)
{
	std::cout << " Candidate received: x=" << message.x << ", y=" << message.y << " from id=" << message.id << std::endl;
	uint32_t candidate_id = message.id;
	double x = message.x;
	double y = message.y;
	bool ib_found = false;
    itt it;
	for( it= m_items.begin(); it != m_items.end(); ++it) 
	{
		ws_ptr mmB = (*it);
		if(euclidean_dist2(x, y, mmB->getX(), mmB->getY()) <=  pow(in_bound, 2))
		{
			if(mmB->getStatus()==Status::clusterHead)
			{
				ib_found = true;
				joinCluster(candidate_id, Status::inBound, mmB->getClusterID(), mmB->getColor());
				break;
			}
			else if((mmB->getStatus()==Status::inBound))
			{
				ib_found = true;
				joinCluster(candidate_id, Status::outBound, mmB->getClusterID(), mmB->getColor());
			}
		}
	}
	if(!ib_found)
		makeCluster(message.id);
}

void WSN::listen_For_ClusterHead(const cluster_head_msg& message)
{
	std::cout << " Cluster Head received: " << message.id << std::endl;
	uint32_t new_cluster_id = message.id;
	double x = message.x;
	double y = message.y;
	std::size_t new_cluster_color = message.color;
	
// 	std::lock_guard<std::mutex> guard(m_mutex);
    itt it;
	for(it=m_items.begin(); it != m_items.end(); ++it)
	{
		ws_ptr mmB = (*it);
		double dist2 = euclidean_dist2(x, y, mmB->getX(), mmB->getY());
		if(dist2>0) // dist2=0 means the same node
		{
			if(dist2 <=  pow(out_bound, 2))
			{
				if(dist2 <=  pow(in_bound, 2))
				{
	// 				if(mmB->getStatus()==Status::idle)
	// 				{
						mmB->setClusterID(new_cluster_id);
						mmB->setStatus(Status::inBound);
						mmB->setColor(new_cluster_color);
	// 				}
				}
				else
				{
					if(mmB->getStatus()==Status::idle)
					{
						mmB->setClusterID(new_cluster_id);
						mmB->setStatus(Status::outBound);
						mmB->setColor(new_cluster_color);
					}
				}
			}
		}
	}
}

void WSN::listen_For_Conflict(const std::string& message)
{
	std::cout << " Conflict received: " << message << std::endl;
}

void WSN::joinCluster(uint32_t id, Status st, uint32_t cluster_id, std::size_t color)
{
	std::lock_guard<std::mutex> guard(m_mutex);
    itt it;
	for(it = m_items.begin(); it != m_items.end(); ++it)
	{
        {
            ws_ptr ws = (*it);
            if(ws->getID()==id)
            {
                ws->setClusterID(cluster_id);
                ws->setStatus(st);
                ws->setColor(color);
    // 			std::shared_ptr<draw_object> new_node = std::make_shared<draw_object>((*it)->getX(), (*it)->getY(), cluster_id);
    // 			m_painter.get()->add_to_draw_queue(new_node);
    // 			m_painter.get()->Enable();
                std::cout << "The BS_" << id << " joined cluster: " << cluster_id << " as " << st << std::endl;
                break;
            }
        }
	}
}

void WSN::makeCluster(uint32_t id)
{
	std::lock_guard<std::mutex> guard(m_mutex);
    itt it;
	for(it=m_items.begin(); it!=m_items.end();++it)
    {
        ws_ptr ws = (*it);
		if(ws->getID()==id)
		{
			ws->setClusterID(id);
			ws->setStatus(Status::clusterHead);
			ws->setColor(generateColor());
			ws->declare_as_cluster_head();
// 			std::shared_ptr<draw_object> new_node = std::make_shared<draw_object>((*it)->getX(), (*it)->getY(), (*it)->getID());
// 			m_painter.get()->add_to_draw_queue(new_node);
// 			m_painter.get()->Enable();
			std::cout << "The BS_" << id << " is Cluster Head!!" << std::endl;
			break;
		}
    }
}


void WSN::generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
{
//     if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(1e6*node_density);
    int num_nodes=pd(m_generator); // Poisson number of points
    //     int num_nodes = 100; // Poisson number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double theta=2*M_PI*(dis(m_generator));   // angular coordinates
        double rho=radius*sqrt(dis(m_generator));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            ws_ptr BS;
            BS = boost::shared_ptr<wSensor>(new wSensor(x,y,get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &IABN::listen_For_parent_update);
//             BS.get()->Start();
// 			BS.get()->candidacy.connect_member(&manager, &IABN::listen_For_Candidacy);
// 			BS.get()->clusterHead.connect_member(&manager, &IABN::listen_For_ClusterHead);
// 			BS.get()->conflict.connect_member(&manager, &IABN::listen_For_Conflict);
        }
    }
}

void WSN::load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
{
    int num_nodes =0;
// 	double data[num_nodes][2];
    std::vector<std::array<double,2>> vec_data;
	std::fstream _file(f_name);

	if(_file.fail())
    {
		std::cerr << "input data file does not exis!" << std::endl;
    }
    std::string line;
	while(std::getline(_file, line) && num_nodes<200)
	{
		std::istringstream is(line);
		double num;
        std::array<double,2> arr;
        is >> arr[0];
        is>>arr[1];
        vec_data.push_back(arr);
        num_nodes++;
	}
	
	
// 	if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    
    num_nodes= vec_data.size(); // number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double x = center_x + 1.5*radius* vec_data[i][0];  // Convert from polar to Cartesian coordinates
        double y = center_y + 1.5*radius* vec_data[i][1];
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            ws_ptr BS;
            BS = boost::shared_ptr<wSensor>(new wSensor(x,y, get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << " at " << __LINE__ <<std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//          BS.get()->Start();
			BS.get()->candidacy.connect_member(this, &WSN::listen_For_Candidacy);
			BS.get()->clusterHead.connect_member(this, &WSN::listen_For_ClusterHead);
			BS.get()->conflict.connect_member(this, &WSN::listen_For_Conflict);
        }
    }
}
