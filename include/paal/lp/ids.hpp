/**
 * @file ids.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-24
 */

#ifndef IDS_HPP
#define IDS_HPP 

namespace paal {
namespace lp {

    class Id {
    protected:
        Id() {}
        explicit Id(int id) : m_id(id) {}
    public:
        int get() const {
            return m_id;
        }

        bool operator<(Id id) const {
            return m_id < id.m_id;
        }
        
        bool operator==(Id id) const {
            return m_id == id.m_id;
        }
    private:
        int m_id;
    };

    struct ColId : Id {
        ColId() {}
        explicit ColId(int id) : Id(id) {} 
    };
    
    inline ColId make_ColId(int col) {
        return ColId(col);
    }

    struct RowId : Id {
        RowId() {}
        explicit RowId(int id) : Id(id) {} 
    };
    
    inline RowId make_RowId(int row) {
        return RowId(row);
    }

} // lp
} // paal

namespace std {

    template <>
    struct hash<paal::lp::Id> {
        size_t operator()(const paal::lp::Id & x) const
        {
            return hash<int>()(x.get());
        }
    };

    template <>
    struct hash<paal::lp::RowId> : public hash<paal::lp::Id> {};

    template <>
    struct hash<paal::lp::ColId> : public hash<paal::lp::Id> {};

} //std

#endif /* IDS_HPP */
