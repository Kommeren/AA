#include <set>
#include <map>
#include <cassert>

namespace paal {
namespace data_structures {

template <typename Vertex>
class FacilityLocationSolution {
    public:
        typedef Vertex VertexType;
        typedef std::set<Vertex> FacilitiesSet;

        FacilityLocationSolution(FacilitiesSet unchosen, FacilitiesSet chosen = FacilitiesSet()) :
            m_chosenFacilities(std::move(chosen)), m_unchosenFacilities(std::move(unchosen)) {}

        const FacilitiesSet & getChosenFacilities() const { 
            return m_chosenFacilities;
        }

        const FacilitiesSet & getUnchosenFacilities() const {
            return m_unchosenFacilities;    
        }

        void add(Vertex v) {
            assert(m_chosenFacilities.find(v) == m_chosenFacilities.end());
            assert(m_unchosenFacilities.find(v) != m_unchosenFacilities.end());
            m_chosenFacilities.insert(v);
            m_unchosenFacilities.erase(v);
        }
        
        void remove(Vertex v) {
            assert(m_chosenFacilities.find(v) != m_chosenFacilities.end());
            assert(m_unchosenFacilities.find(v) == m_unchosenFacilities.end());
            m_chosenFacilities.erase(v);
            m_unchosenFacilities.insert(v);
        }

    protected:
        FacilitiesSet m_chosenFacilities;
        FacilitiesSet m_unchosenFacilities;
};


template <typename Vertex, typename Metric, typename FacilityCost>
class FacilityLocationSolutionWithClientsAssignment : 
    public FacilityLocationSolution<Vertex> {
    public:
        typedef FacilityLocationSolution<Vertex> base; 
        using base::VertexType;
        typedef typename Metric::DistanceType Dist;
        typedef typename base::FacilitiesSet FacilitiesSet;
        //TODO change to vector
        typedef FacilitiesSet ClientsSet;
        using base::m_chosenFacilities;
        using base::m_unchosenFacilities;

        FacilityLocationSolutionWithClientsAssignment(
                                 FacilitiesSet unchosen,
                                 const FacilitiesSet & chosen, 
                                 ClientsSet clients,
                                 Metric & m,
                                 FacilityCost & c) :
            base(std::move(unchosen)), m_clients(std::move(clients)), m_metric(m), m_facCosts(c) {
                for(Vertex f : m_chosenFacilities) {
                    addFacility(f);
                }
            }

       
        // returns diff between new cost and old cost
        Dist addFacility(Vertex f) {
            Dist cost = m_facCosts(f);
            base::add(f);
           
            //first facility
            if(m_chosenFacilities.size() == 1) {
                m_clientsToFac.clear();
                m_facToClients.clear();
                for(Vertex v : m_clients) {
                    m_clientsToFac[v] = 
                        m_facToClients.insert(std::make_pair(f, v));
                    cost += m_metric(v,f); 
                }
                
                //could be too tricky
                cost = -cost;
                
            } else {
                for(Vertex v: m_clients) {
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
        Dist remFacility(Vertex f) {
            Dist cost = -m_facCosts(f);
            if(m_chosenFacilities.size() == 1) {
                //could be to tricky...
                cost = Dist();
                for(Vertex v : m_clients) {
                    cost += dist(v);
                }
                m_clientsToFac.clear();
                m_facToClients.clear();
            } else {

                auto op = std::bind(std::not_equal_to<Vertex>(), f, std::placeholders::_1);
                auto begin = m_facToClients.lower_bound(f);
                auto end = m_facToClients.upper_bound(f);
                for(;begin != end; ) {
                    auto v = begin->second;
                    //using the fact that this is a map 
                    //(with other containers you have to be carefull cause of iter invalidation)
                    ++begin;
                    cost -= dist(v);
                    cost += adjustClient(v, op);
                }
            }
            base::remove(f);
            return cost;
        }

    private:
        
        Dist dist(Vertex v) {
            return m_metric(v, clientToFac(v));
        }
        
        Dist adjustClient(Vertex v, std::function<bool(Vertex)> filter = [](Vertex v){return true;}) {
            bool init = true;
            Dist d;
            for(Vertex f : m_chosenFacilities) {
                if(filter(f) &&  (init || m_metric(v,f) < d)) {
                    assign(v,f);
                    d = m_metric(v,f);
                    init = false;
                }
            }
            assert(!init);
            return d; 
        }

        Vertex clientToFac(Vertex v) const {
            auto i = m_clientsToFac.find(v);
            assert(i != m_clientsToFac.end());
            return i->second->first;
        }

        void assign(Vertex v, Vertex f) {
            auto prev = m_clientsToFac[v];
            m_facToClients.erase(prev);
            m_clientsToFac[v] = 
                m_facToClients.insert(std::make_pair(f, v));
        }

        typedef std::multimap<Vertex, Vertex> FacilitiesToClients;
        typedef std::map<Vertex, 
                typename FacilitiesToClients::iterator> ClientsToFacilities;
        
        ClientsToFacilities m_clientsToFac;
        FacilitiesToClients m_facToClients;
        ClientsSet m_clients;
        Metric & m_metric;
        FacilityCost & m_facCosts;
};

};
};
