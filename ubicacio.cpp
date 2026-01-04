#include "ubicacio.hpp"

//Cost: O(1)
ubicacio::ubicacio(int i, int j, int k) {
    if ((i >= 0 or j >= 0 or k >= 0) or (i == -1 and j == 0 and k == 0) or (i == -1 and j == -1 and k == -1)) {
        _i = i;
        _j = j;
        _k = k;
    }
    else {
        throw error(UbicacioIncorrecta);
    }
}

//Cost: O(1)
ubicacio::ubicacio(const ubicacio& u) {
    _i = u._i;
    _j = u._j;
    _k = u._k;
}

//Cost: O(1)
ubicacio& ubicacio::operator=(const ubicacio& u) {
    if (this != &u) {
        _i = u._i;
        _j = u._j;
        _k = u._k;
    }
    return *this;
}

//Cost: O(1)
ubicacio::~ubicacio() noexcept {}

//Cost: O(1)
int ubicacio::filera() const noexcept {
    return _i;
}

//Cost: O(1)
int ubicacio::placa() const noexcept {
    return _j;
}



//Cost: O(1)
int ubicacio::pis() const noexcept {
    return _k;
}

//Cost: O(1)
bool ubicacio::operator==(const ubicacio& u) const noexcept {
    bool es_igual;
    if (_i == u._i and _j == u._j and _k == u._k) es_igual = true;
    else es_igual = false;
    return es_igual;
}

//Cost: O(1)
bool ubicacio::operator!=(const ubicacio& u) const noexcept {
    return !(*this == u);
}

//Cost: O(1)
bool ubicacio::operator<(const ubicacio& u) const noexcept {
    bool menys = false;
    if (_i < u._i or (_i == u._i and _j < u._j) or (_i == u._i and _j == u._j and _k < u._k)) menys = true;
    return menys;
}

//Cost: O(1)
bool ubicacio::operator<=(const ubicacio& u) const noexcept {
    return !(*this > u);
}

//Cost: O(1)
bool ubicacio::operator>(const ubicacio& u) const noexcept {
    bool mes = false;
    if (_i > u._i or (_i == u._i and _j > u._j) or (_i == u._i and _j == u._j and _k > u._k)) mes = true;
    return mes;
}

//Cost: O(1)
bool ubicacio::operator>=(const ubicacio& u) const noexcept {
    return !(*this < u);
}

