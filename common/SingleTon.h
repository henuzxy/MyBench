template<typename T>
class SingleTon{
public:
    SingleTon() = delete;
    SingleTon(const SingleTon&) = delete;

private:
    static T *ptr;
};