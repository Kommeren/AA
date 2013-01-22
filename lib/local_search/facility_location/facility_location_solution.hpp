#include <set>
#include <map>
#include <cassert>

namespace paal {
namespace local_search {
namespace facility_location {


template <typename VertexType>
class FacilityLocationSolution {
    public:
        typedef std::set<VertexType> FacilitiesSet;

        FacilityLocationSolution(const FacilitiesSet & unchosen, const FacilitiesSet & chosen = FacilitiesSet()) :
            m_chosenFacilities(chosen), m_unchosenFacilities(unchosen) {}

        const FacilitiesSet & getChosenFacilities() { 
            return m_chosenFacilities;
        }

        const FacilitiesSet & getUnchosenFacilities() {
            return m_unchosenFacilities;    
        }

        void add(VertexType v) {
            assert(m_chosenFacilities.find(v) == m_chosenFacilities.end());
            assert(m_unchosenFacilities.find(v) != m_unchosenFacilities.end());
            m_chosenFacilities.insert(v);
            m_unchosenFacilities.erase(v);
        }
        
        void remove(VertexType v) {
            assert(m_chosenFacilities.find(v) != m_chosenFacilities.end());
            assert(m_unchosenFacilities.find(v) == m_unchosenFacilities.end());
            m_chosenFacilities.erase(v);
            m_unchosenFacilities.insert(v);
        }

    protected:
        FacilitiesSet m_chosenFacilities;
        FacilitiesSet m_unchosenFacilities;
};


template <typename VertexType, typename Metric, typename FacilityCost>
class FacilityLocationSolutionWithClientsAssignment : 
        protected FacilityLocationSolution<VertexType> {
    public:
        typedef FacilityLocationSolution<VertexType> base; 
        typedef typename Metric::DistanceType Dist;
        typedef typename base::FacilitiesSet FacilitiesSet;
        typedef FacilitiesSet ClientsSet;
        using base::m_chosenFacilities;
        using base::m_unchosenFacilities;

        FacilityLocationSolutionWithClientsAssignment(
                                 const FacilitiesSet & unchosen,
                                 const FacilitiesSet & chosen, 
                                 const ClientsSet & clients,
                                 Metric & m,
                                 FacilityCost & c) :
            base(unchosen, chosen), m_clients(clients), m_metric(m), m_facCosts(c) {
                for(VertexType f : m_chosenFacilities) {
                    addFacility(f);
                }
            }

        Dist dist(VertexType v) {
            return m_metric(v, clientToFac(v));
        }
       
        // returns diff between new cost and old cost
        Dist addFacility(VertexType f) {
            Dist cost = m_facCosts(f);
            base::add(f);
           
            //first facility
            if(m_chosenFacilities.size() == 1) {
                m_clientsToFac.clear();
                m_facToClients.clear();
                for(VertexType v : m_clients) {
                    m_clientsToFac[v] = 
                        m_facToClients.insert(std::make_pair(f, v));
                    cost += m_metric(v,f); 
                }
                
                //could be too tricky
                cost = -cost;
                
            } else {
                for(VertexType v: m_clients) {
                    Dist d = m_metric(v,f) - dist(v);
                    if(d < 0) {
                        cost += d;
                        assign(v,f);
                    }
                }
            }
            return cost;
        }
        
        // returns diff between new cost and old cost
        Dist remFacility(VertexType f) {
            Dist cost = -m_facCosts(f);
            auto op = std::bind(std::not_equal_to<VertexType>(), f, std::placeholders::_1);
            std::for_each(m_facToClients.lower_bound(f), m_facToClients.upper_bound(f), [&](const std::pair<VertexType,VertexType> & v) {
                    std::cout << "d " << cost << " "<< f << std::endl;
                cost -= this->dist(v.second);
                    std::cout << "d1 " << cost << std::endl;
                cost += this->adjustClient(v.second, op);
                    std::cout << "d2 " << cost << std::endl;
            });
            base::remove(f);
            return cost;
        }

    private:
        
        Dist adjustClient(VertexType v, std::function<bool(VertexType)> filter = [](VertexType v){return true;}) {
            bool init = true;
            Dist d;
            for(VertexType f : m_chosenFacilities) {
                if(filter(f)) {
                    if(init || m_metric(v,f) < d) {
                        assign(v,f);
                        d = m_metric(v,f);
                        init = false;
                    }
                }
            }
            assert(!init);
            return d; 
        }

        VertexType clientToFac(VertexType v) const {
            auto i = m_clientsToFac.find(v);
            assert(i != m_clientsToFac.end());
            return i->first;
        }

        void assign(VertexType v, VertexType f) {
            auto prev = m_clientsToFac[v];
            m_facToClients.erase(prev);
            m_clientsToFac[v] = 
                m_facToClients.insert(std::make_pair(f, v));
        }

        typedef std::multimap<VertexType, VertexType> FacilitiesToClients;
        typedef std::map<VertexType, 
                typename FacilitiesToClients::iterator> ClientsToFacilities;
        
        ClientsToFacilities m_clientsToFac;
        FacilitiesToClients m_facToClients;
        ClientsSet m_clients;
        Metric & m_metric;
        FacilityCost & m_facCosts;
};

};
};
};
