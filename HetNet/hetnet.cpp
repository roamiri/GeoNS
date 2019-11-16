#include "hetnet.h"

HetNet::HetNet():Container()
{

}

HetNet::~HetNet()
{

}

void HetNet::generate_fixed_nodes(int count)
{
}

void HetNet::generate_MBS(double node_density, bool fixed, int fixed_count)
{
    // Adding Fixed Wired BSs
    double delta_teta = 2*M_PI/fixed_count;
    for(int i =0;i<fixed_count;i++)
    {
        mbs_ptr BS;
        double theta = i*delta_teta;
        double r2 = radius/4.;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        BS = boost::shared_ptr<MacroBS>(new MacroBS(x, y, get_nextID()));
        BS->set_transmit_power(def_P_tx);
        BS.get()->setColor(0);
        BS->set_backhaul_Type(Backhaul::wired);
        BS->set_hop_count(0);
        BS->route_found(true);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
//        m_items.push_back(BS);
    }
}

void HetNet::generate_SBS(double node_density, bool fixed, int fixed_count)
{
    // Adding Fixed SBSs
    double delta_teta = 2*M_PI/fixed_count;
    for(int i =0;i<fixed_count;i++)
    {
        sbs_ptr BS;
        double theta = i*delta_teta;
        double r2 = radius/2.;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        BS = boost::shared_ptr<smallBS>(new smallBS(x, y, get_nextID()));
        BS->set_transmit_power(def_P_tx);
        BS.get()->setColor(0);
        BS->set_backhaul_Type(Backhaul::wireless);
        BS->set_hop_count(0);
        BS->route_found(true);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
//        m_items.push_back(BS);
    }
}

void HetNet::generate_UEs(double node_density, bool fixed, int fixed_count)
{
    // Adding Fixed Users
    double delta_teta = 2*M_PI/fixed_count;
    for(int i =0;i<fixed_count;i++)
    {
        user_ptr UE;
        double theta = i*delta_teta;
        double r2 = radius/1.2;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        UE = boost::shared_ptr<User>(new User(x, y, get_nextID()));
        UE->set_transmit_power(def_P_tx);
        UE.get()->setColor(0);
        UE->set_hop_count(0);
        UE->route_found(true);
        m_tree.insert(std::make_pair(UE->get_loc(), UE));
//        m_items.push_back(BS);
    }
}

void HetNet::load_MBS(double node_density, bool fixed, int fixed_count)
{
    
}

void HetNet::load_SBS(double node_density, bool fixed, int fixed_count)
{
    
}

void HetNet::load_UEs(double node_density, bool fixed, int fixed_count)
{
    
}

void HetNet::generate_Blockage()
{
    bl_ptr block;
    double theta = M_PI_2;
    double r2 = radius/1.5;
    double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
    double y = center_y + r2 * sin(theta);
    float width=10.;
    float height=10.;
    block = boost::shared_ptr<Blockage>(new Blockage(x, y, width, height, get_nextID()));
    
    m_tree.insert(std::make_pair(block->get_loc(), block));
}

void HetNet::load_Blockage()
{
    
}

void HetNet::generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
{
    
}
    
    
void HetNet::load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
{
    
}
