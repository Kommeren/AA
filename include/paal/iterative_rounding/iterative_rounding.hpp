#ifndef ITERATIVE_ROUNDING_HPP
#define ITERATIVE_ROUNDING_HPP 

#include <cstdlib>
#include <string>
#include <map>
#include <set>

#include <boost/range/irange.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/iterative_rounding/glp_lpbase.hpp"

namespace paal {

template <typename Engine, typename LPBase = GLPBase>
class IterativeRounding  {
public:
    IterativeRounding(Engine e) : m_engine(std::move(e)) {
        e.init(m_lpBase);
    }
    double solve() {
        return m_lpBase.solve();
    }
    
    bool round() {
        int delelted(0);
        int size = m_lpBase.colSize();
        LOG("roundGen");
        LOG("size = " << size);
        for(int i = 1; i <= size; ++i) {
            auto doRound = m_engine.roundCondition(m_lpBase, i);
            if(doRound.first) {
                LOG("rounduje = " << i);
                roundColToValue(i, doRound.second);
                LOG("porounduje = " << i);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }
    
    bool relax() {
        int delelted(0);
        int size = m_lpBase.rowSize();
        for(int i = 1; i <= size; ++i) {
            if(m_engine.relaxCondition(m_lpBase, i)) {
                LOG("RELAKSUJE " << i);
                m_lpBase.deleteRow(i);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }


private:    
   
    void roundColToValue(int col, double value) {
        LOG("round na col = " << col << " val =  " << value ); 
        auto column = m_lpBase.getColumn(col);
        int row;
        double coef;
        for(const boost::tuple<int, double> & c : utils::make_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = m_lpBase.getRowUb(row);
            double currLb = m_lpBase.getRowLb(row);
            BoundType currType = m_lpBase.getRowBoundType(row);
            double diff = coef * value;
            LOG("Usataiwam bounda na wiersz " << row << " diff = " << diff); 
            m_lpBase.setRowBounds(row, currType, currLb - diff, currUb - diff);
        }
        m_lpBase.deleteRow(col);
    }
    
    LPBase m_lpBase;
    Engine m_engine;
};

} //paal


#endif //ITERATIVE_ROUNDING_HPP 
