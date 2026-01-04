#include "terminal.hpp"

// **************************************************
// *****************METODES PRIVATS******************
// **************************************************

ubicacio terminal::buscar_first_fit(nat longitud) const {
    ubicacio u(-1, -1, -1);
    bool trobat = false;
    for (nat i = 0; i < _files and not trobat; i++) {
        for (nat j = 0; j < _places and not trobat; j++) {
            for (nat k = 0; k < _altura and not trobat; k++) {
                if (espai_disponible(i, j, k, longitud)) {
                    u = ubicacio(i, j, k);
                    trobat = true;
                }
            }
        }
    }
    return u;
}

bool terminal::espai_disponible(nat fila, nat placa, nat altura, nat longitud) const {
    bool espai = true;
    nat num_places = longitud / 10;
    if (placa + num_places > _places) espai = false
    if (espai) {
        for (nat p = placa; p < placa + num_places; ++p) {
            if (_term[fila][p][altura] != "") espai = false;
        }
    }
    if (espai && altura > 0) {
        for (nat p = placa; p < placa + num_places; ++p) {
            if (_term[fila][p][altura - 1] == "") espai = false;
        }
    }
    return espai;
}


void terminal::colocar_contenidor(contenidor& c, ubicacio& u) {
    nat num_places = c.longitud() / 10;
    nat fila = u.filera();
    nat placa = u.placa();
    nat altura = u.pis();
    for (int i = placa; i < placa + num_places; i++) _term[fila][i][altura] = c.matricula();
    _contenidors.assig(c.matricula(), c);
    _ops_grua++;
}

void terminal::processar_area_espera() {
    bool colocat = true;
    while (colocat and not _area_esp.empty()) {
        colocat = false;
        auto it = _area_esp.rbegin();
        while (it != _area_esp.rend()) {
            const string matricula = *it;
            const contenidor conte = _contenidors[matricula];
            ubicacio u(-1, -1, -1);
            u = buscar_first_fit(conte.longitud());
            if (u.filera() != -1) {
                nat num_places = conte.longitud() / 10;
                for (nat i = u.placa(); i < u.placa() + num_places; i++) _term[u.filera()][i][u.pis()] = matricula;
                _ops_grua++;
                ++it;                                           //incrementem perque quan es crea amb .base(), apunta una posicio endavant del lloc de reverse_iterator (ex: si tenim [A, B, C, D] i reverse_iterator apunta a C, al fer .base() aquest apuntara a D. Llavors ens movem a B i .base() estarà a C.
                auto it_aux = it.base();        //.base() fa un iterador normal a partir del reverse iterator it. Ho fem perque .erase() no funciona amb reverse_iterator                                               
                _area_esp.erase(it_aux);
                colocat = true;
            }
            else {
                ++it;
            }
        }
    }
}



// **************************************************
// *****************METODES PUBLICS******************
// **************************************************

terminal::terminal(nat n, nat m, nat h, estrategia st) : _contenidors(n* m* h) {
    if (n == 0) throw error(NumFileresIncorr);
    if (m == 0) throw error(NumPlacesIncorr);
    if (h == 0 or h > HMAX) throw error(AlcadaMaxIncorr);
    if (st != estrategia::FIRST_FIT && st != estrategia::LLIURE) throw error(EstrategiaIncorr);
    _files = n;
    _places = m;
    _altura = h;
    _estrategia = st;
    _ops_grua = 0;

    _term = new string * *[_files];
    for (nat i = 0; i < _files; i++) {
        _term[i] = new string * [_places];
        for (nat j = 0; j < _places; j++) {
            _term[i][j] = new string[_altura];
            for (nat k = 0; k < _altura; k++) {
                _term[i][j][k] = "";
            }
        }
    }
}

terminal::terminal(const terminal& b) : _contenidors(b._contenidors) {
    _files = b._files;
    _places = b._places;
    _altura = b._altura;
    _estrategia = b._estrategia;
    _ops_grua = b._ops_grua;
    _area_esp = b._area_esp;

    _term = new string * *[_files];
    for (nat i = 0; i < _files; i++) {
        _term[i] = new string * [_places];
        for (nat j = 0; j < _places; j++) {
            _term[i][j] = new string[_altura];
            for (nat k = 0; k < _altura; k++) {
                _term[i][j][k] = b._term[i][j][k];
            }
        }
    }
}

terminal& terminal::operator=(const terminal& b) {
    if (this != &b) {
        for (nat i = 0; i < _files; i++) {
            for (nat j = 0; j < _places; j++) {
                delete[] _term[i][j];
            }
            delete[] _term[i];
        }
        delete[] _term;

        _files = b._files;
        _places = b._places;
        _altura = b._altura;
        _estrategia = b._estrategia;
        _ops_grua = b._ops_grua;
        _area_esp = b._area_esp;
        _contenidors = b._contenidors;

        _term = new string * *[_files];
        for (nat i = 0; i < _files; i++) {
            _term[i] = new string * [_places];
            for (nat j = 0; j < _places; j++) {
                _term[i][j] = new string[_altura];
                for (nat k = 0; k < _altura; k++) {
                    _term[i][j][k] = b._term[i][j][k];
                }
            }
        }
    }
    return *this;
}

terminal::~terminal() noexcept {
    for (nat i = 0; i < _files; i++) {
        for (nat j = 0; j < _places; j++) {
            delete[] _term[i][j];
        }
        delete[] _term[i];
    }
    delete[] _term;
}

void terminal::insereix_contenidor(const contenidor& c) {

}

void terminal::retira_contenidor(const string& m) {

}

ubicacio terminal::on(const string& m) const noexcept {
    ubicacio ub_contenidor(-1, -1, -1);

    bool a_espera = false;
    for (auto it = _area_esp.begin(); it != _area_esp.end() && !a_espera; ++it) {
        if (*it == m) {
            ub_contenidor = ubicacio(-1, 0, 0);
            a_espera = true;
        }
    }

    if (!a_espera) {
        bool trobat = false;
        for (nat i = 0; i < _files && !trobat; ++i) {
            for (nat j = 0; j < _places && !trobat; ++j) {
                for (nat k = 0; k < _altura && !trobat; ++k) {
                    if (_term[i][j][k] == m) {
                        ub_contenidor = ubicacio(i, j, k);
                        trobat = true;
                    }
                }
            }
        }
    }

    return ub_contenidor;
}


nat terminal::longitud(const string& m) const {
    if (_contenidors.existeix(m) == false) throw error(MatriculaInexistent);
    return _contenidors[m].longitud();
}

void terminal::contenidor_ocupa(const ubicacio& u, string& m) const {
    int i = u.filera();
    int j = u.placa();
    int k = u.pis();
    if (i < 0 || j < 0 || k < 0 ||
        i >= (int)_files || j >= (int)_places || k >= (int)_altura) {
        throw error(UbicacioNoMagatzem);
    }
    m = _term[i][j][k];
}


nat terminal::fragmentacio() const noexcept {
    nat frag = 0;
    for (nat f = 0; f < _files; ++f) {
        for (nat k = 0; k < _altura; ++k) {
            nat j = 0;
            while (j < _places) {
                auto usable = [&](nat pos) -> bool {
                    if (_term[f][pos][k] != "") return false;
                    if (k == 0) return true;
                    return _term[f][pos][k - 1] != "";
                };
                if (!usable(j)) {
                    ++j;
                }
                else {
                    nat start = j;
                    while (j < _places && usable(j)) ++j;
                    nat len = j - start;
                    if (len == 1) frag += 1;
                }
            }
        }
    }
    return frag;
}


nat terminal::ops_grua() const noexcept {
    return _ops_grua;
}

void terminal::area_espera(list<string>& l) const noexcept {
    l = _area_esp;
    l.sort();      //mètode de std::list que està permès per a l'àrea d'espera
}

nat terminal::num_fileres() const noexcept {
    return _files;
}

nat terminal::num_places() const noexcept {
    return _places;
}

nat terminal::num_pisos() const noexcept {
    return _altura;
}

estrategia terminal::quina_estrategia() const noexcept {
    return _estrategia;
}



