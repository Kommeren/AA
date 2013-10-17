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
namespace ir {

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

    ColId make_ColId(int col) {
        return ColId(col);
    }

    struct RowId : Id {
        RowId() {}
        explicit RowId(int id) : Id(id) {} 
    };
    
    RowId make_RowId(int row) {
        return RowId(row);
    }

} // ir
} // paal

namespace std {

    template <>
    struct hash<paal::ir::Id> {
        size_t operator()(const paal::ir::Id & x) const
        {
            return hash<int>()(x.get());
        }
    };

    template <>
    struct hash<paal::ir::RowId> : public hash<paal::ir::Id> {};

    template <>
    struct hash<paal::ir::ColId> : public hash<paal::ir::Id> {};

} //std

#endif /* IDS_HPP */
