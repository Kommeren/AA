
enum {REMOVE, ADD, SWAP} UpdateTypes;

class AbstractUpdate {
protected:
    virtual ~AbstractUpdate() {}
public:
    virtual UpdateTypes getType() = 0;
};

class Swap : AbstractUpdate {
    UpdateTypes getType() {
        return SWAP;
    }

};
