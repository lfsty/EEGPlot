#ifndef STDAFX_H
#define STDAFX_H

#define PROPERTY_AUTO(TYPE, M)                                                                                              \
    public:                                                                                                                 \
    void set##M(const TYPE& in_##M)                                                                                         \
    {                                                                                                                       \
        m_##M = in_##M;                                                                                                     \
    }                                                                                                                       \
    const TYPE get##M() const                                                                                               \
    {                                                                                                                       \
        return m_##M;                                                                                                       \
    }                                                                                                                       \
    private:                                                                                                                \
    TYPE m_##M;

#endif // STDAFX_H
