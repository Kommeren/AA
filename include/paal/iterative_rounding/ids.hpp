#ifndef IDS_HPP
#define IDS_HPP 

namespace paal {
namespace ir {

    class Id {
    protected:
        Id() {}
        explicit Id(int id) : m_id(id) {}
    public:
        int get() {
            return m_id;
        }

        bool operator<(Id id) const {
            return m_id < id.m_id;
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
#endif /* IDS_HPP */
