
#include "contenidor.hpp"


contenidor::contenidor(const string& m, nat l) {
    bool m_correcte = true;
    nat i = 0;
    if (m.size() == 0) throw error(MatriculaIncorrecta);
    while (i < m.size() and m_correcte) {
        if (('A' > m[i] and ('0' > m[i] or '9' < m[i])) or ('Z' < m[i])) m_correcte = false;
        i++;
    }
    if (m_correcte == false) throw error(MatriculaIncorrecta);
    if (l != 10 and l != 20 and l != 30) {
        throw error(LongitudIncorrecta);
    }
    _matricula = m;
    _longitud = l;
}

contenidor::contenidor(const contenidor& u) {
    _longitud = u._longitud;
    _matricula = u._matricula;
}

contenidor& contenidor::operator=(const contenidor& u) {
    if (this != &u) {
        _longitud = u._longitud;
        _matricula = u._matricula;
    }
    return *this;
}

contenidor::~contenidor() noexcept {}


nat contenidor::longitud() const noexcept {
    return _longitud;
}

string contenidor::matricula() const noexcept {
    return _matricula;
}


bool contenidor::operator==(const contenidor& c) const noexcept {
    bool es_igual = true;
    if (_longitud != c._longitud or _matricula != c._matricula) es_igual = false;
    return es_igual;
}

bool contenidor::operator!=(const contenidor& c) const noexcept {
    return !(*this == c);
}

bool contenidor::operator<(const contenidor& c) const noexcept {
    bool es_menys = false;
    if (_matricula < c._matricula or (_matricula == c._matricula and _longitud < c._longitud)) es_menys = true;
    return es_menys;
}

bool contenidor::operator<=(const contenidor& c) const noexcept {
    return !(*this > c);
}

bool contenidor::operator>(const contenidor& c) const noexcept {
    bool es_mes = false;
    if (_matricula > c._matricula or (_matricula == c._matricula and _longitud > c._longitud)) es_mes = true;
    return es_mes;
}

bool contenidor::operator>=(const contenidor& c) const noexcept {
    return !(*this < c);
}

