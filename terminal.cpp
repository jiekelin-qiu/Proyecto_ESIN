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

ubicacio terminal::buscar_estrat_lliure(nat longitud) const {
    return buscar_first_fit(longitud);
}

bool terminal::espai_disponible(nat fila, nat placa, nat altura, nat longitud) const {
    bool espai = true;
    nat num_places = longitud / 10;
    if (placa + num_places > _places) espai = false;
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
    while (colocat and not area_esp.empty()) {
        colocat = false;
        list<string>::reverse_iterator it = area_esp.rbegin();
        while (it != area_esp.rend()) {
            const string matricula = *it;
            const contenidor conte = _contenidors[matricula];
            ubicacio u(-1, -1, -1);
            u = buscar_first_fit(conte.longitud());
            if (u.filera() != -1) {
                nat num_places = conte.longitud() / 10;
                for (nat i = u.placa(); i < u.placa() + num_places; i++) _term[u.filera()][i][u.pis()] = matricula;
                _ops_grua++;
                ++it;                                           //incrementem perque quan es crea amb .base(), apunta una posicio endavant del lloc de reverse_iterator (ex: si tenim [A, B, C, D] i reverse_iterator apunta a C, al fer .base() aquest apuntara a D. Llavors ens movem a B i .base() estaria C.
                list<string>::iterator it_aux = it.base();        //.base() fa un iterador normal a partir del reverse iterator it. Ho fem perque .erase() no funciona amb reverse_iterator                                               
                area_esp.erase(it_aux);
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
    area_esp = b.area_esp;

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
        area_esp = b.area_esp;
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
    const string m = c.matricula();
    const ubicacio u_noex(-1, -1, -1);
    if(!(on(m) == u_noex)) throw error(MatriculaDuplicada);
    ubicacio u(-1, -1, -1);
    if (_estrategia == estrategia::FIRST_FIT) {
        u = buscar_first_fit(c.longitud());
    } else {
        u = buscar_estrat_lliure(c.longitud());
    }
    if(u.filera() == -1) {
        _contenidors.assig(m, c);
        area_esp.push_back(m);
        return;
    }
    contenidor c_copia = c;
    ubicacio u_copia = u;
    colocar_contenidor(c_copia, u_copia);
    processar_area_espera();
}

void terminal::retira_contenidor(const string &m) {
    ubicacio u = on(m);
    if (u == ubicacio(-1, -1, -1)) {
        throw error(MatriculaInexistent);
    }
    if (u == ubicacio(-1, 0, 0)) {
        list<string>::iterator it = area_esp.begin();
        bool trobat = false;
        while (it != area_esp.end() && !trobat) {
            if (*it == m) {
                it = area_esp.erase(it);
                trobat = true;
            } else {
                ++it;
            }
        }
        _contenidors.elimina(m);
        return;
    }
    list<string> need;
    need.push_back(m);
    bool afegit = true;
    while (afegit) {
        afegit = false;
        list<string>::iterator itn = need.begin();
        while (itn != need.end()) {
            string x = *itn;
            ubicacio ux = on(x);
            if (ux == ubicacio(-1, 0, 0)) {
                ++itn;
                continue;
            }
            nat fi = (nat)ux.filera();
            nat pj = (nat)ux.placa();
            nat pk = (nat)ux.pis();

            contenidor cx = _contenidors[x];
            nat np = cx.longitud() / 10;
            if (pk + 1 < _altura) {
                for (nat p = pj; p < pj + np; ++p) {
                    string y = _term[fi][p][pk + 1];
                    if (y != "") {
                        bool ja = false;
                        list<string>::iterator itc = need.begin();
                        while (itc != need.end() && !ja) {
                            if (*itc == y) ja = true;
                            ++itc;
                        }
                        if (!ja) {
                            need.push_back(y);
                            afegit = true;
                        }
                    }
                }
            }

            ++itn;
        }
    }
    bool objectiu_lliure = false;
    while (!objectiu_lliure) {
        ubicacio um = on(m);
        nat f = (nat)um.filera();
        nat j = (nat)um.placa();
        nat k = (nat)um.pis();
        contenidor cm = _contenidors[m];
        nat np_m = cm.longitud() / 10;
        bool hiha_sobre = false;
        if (k + 1 < _altura) {
            for (nat p = j; p < j + np_m && !hiha_sobre; ++p) {
                if (_term[f][p][k + 1] != "") hiha_sobre = true;
            }
        }
        if (!hiha_sobre) objectiu_lliure = true;
        if (objectiu_lliure) break;
        bool hiha_candidat = false;
        string millor = "";
        ubicacio ub_millor(0, 0, 0);
        list<string>::iterator it = need.begin();
        while (it != need.end()) {
            string x = *it;
            if (x == m) { ++it; continue; }
            ubicacio ux = on(x);
            if (ux == ubicacio(-1, 0, 0) || ux == ubicacio(-1, -1, -1)) {
                ++it;
                continue;
            }
            nat fi = (nat)ux.filera();
            nat pj = (nat)ux.placa();
            nat pk = (nat)ux.pis();
            contenidor cx = _contenidors[x];
            nat np = cx.longitud() / 10;
            bool lliure = true;
            if (pk + 1 < _altura) {
                for (nat p = pj; p < pj + np; ++p) {
                    if (_term[fi][p][pk + 1] != "") lliure = false;
                }
            }
            if (lliure) {
                if (!hiha_candidat) {
                    hiha_candidat = true;
                    millor = x;
                    ub_millor = ux;
                } else {
                    if (ux < ub_millor) {
                        millor = x;
                        ub_millor = ux;
                    }
                }
            }
            ++it;
        }
        if (!hiha_candidat) {
            break;
        }
        ubicacio ux = on(millor);
        nat fi = (nat)ux.filera();
        nat pj = (nat)ux.placa();
        nat pk = (nat)ux.pis();
        contenidor cx = _contenidors[millor];
        nat np = cx.longitud() / 10;
        for (nat p = pj; p < pj + np; ++p) {
            _term[fi][p][pk] = "";
        }
        area_esp.push_back(millor);
        _ops_grua++;
        list<string>::iterator itd = need.begin();
        bool esborrat = false;
        while (itd != need.end() && !esborrat) {
            if (*itd == millor) {
                itd = need.erase(itd);
                esborrat = true;
            } else {
                ++itd;
            }
        }
    }
    ubicacio um = on(m);
    nat f = (nat)um.filera();
    nat j = (nat)um.placa();
    nat k = (nat)um.pis();
    contenidor cm2 = _contenidors[m];
    nat np2 = cm2.longitud() / 10;
    for (nat p = j; p < j + np2; ++p) {
        _term[f][p][k] = "";
    }
    _contenidors.elimina(m);
    _ops_grua++; 
    processar_area_espera();
}


ubicacio terminal::on(const string& m) const noexcept {
    ubicacio ub_contenidor(-1, -1, -1);

    bool a_espera = false;
    list<string>::const_iterator it = area_esp.begin();
    while(it != area_esp.end() and !a_espera) {
        if(*it == m) {
            ub_contenidor = ubicacio(-1, 0, 0);
            a_espera = true;
        }
        ++it;
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
                bool es_usable = false;
                if (_term[f][j][k] == "") {
                    if (k == 0) {
                        es_usable = true;
                    } else {
                        if (_term[f][j][k - 1] != "") es_usable = true;
                    }
                }
                if (!es_usable) {
                    ++j;
                } else {
                    nat start = j;
                    while (j < _places) {
                        bool usable2 = false;
                        if (_term[f][j][k] == "") {
                            if (k == 0) {
                                usable2 = true;
                            } else {
                                if (_term[f][j][k - 1] != "") usable2 = true;
                            }
                        }
                        if (!usable2) break;
                        ++j;
                    }
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
    l = area_esp;
    l.sort();      //metode de std::list que esta permes per a l'area d'espera
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

terminal::estrategia terminal::quina_estrategia() const noexcept {
    return _estrategia;
}



