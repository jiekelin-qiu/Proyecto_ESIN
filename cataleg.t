template <typename Valor>
cataleg<Valor>::cataleg(nat numelems) {
    _M = numelems*4/3;
    if(_M==0) _M=1;
    _taula = new node_hash*[_M];
    for (nat i=0; i<_M; i++) _taula[i]=nullptr;
    _quants=0;
}

template <typename Valor>
cataleg<Valor>::cataleg(const cataleg& c) {
    _M=c._M;
    _quants=c._quants;
    _taula=new node_hash*[_M];
    for (nat i=0; i<_M; i++) {
        _taula[i]=nullptr;
    }
    for (nat i=0; i<_M; i++) {
        node_hash *posicio=c._taula[i];
        while (posicio!=nullptr) {
            node_hash *aux=new node_hash{posicio->clau, posicio->valor, nullptr};
        if (_taula[i]==nullptr) _taula[i]=aux;
        else {
            node_hash *aux2=_taula[i];
            while (aux2->seg!=nullptr) aux2=aux2->seg;
            aux2->seg=aux;
        }
        posicio=posicio->seg;
        }
    }
}

template <typename Valor>
cataleg<Valor>& cataleg<Valor>::operator=(const cataleg& c) {
    if (this != &c) {
        for (nat i = 0; i < _M; ++i) {
            node_hash* actual = _taula[i];
            while (actual != nullptr) {
                node_hash* seg = actual->seg;
                delete actual;
                actual = seg;
            }
        }
        delete[] _taula;
    
        _M=c._M;
        _quants=c._quants;
        _taula=new node_hash*[_M];
        for (nat i=0; i<_M; i++) {
            _taula[i]=nullptr;
        }
        for (nat i=0; i<_M; i++) {
            node_hash *posicio=c._taula[i];
            while (posicio!=nullptr) {
                node_hash *aux=new node_hash{posicio->clau, posicio->valor, nullptr};
            if (_taula[i]==nullptr) _taula[i]=aux;
            else {
                node_hash *aux2=_taula[i];
                while (aux2->seg!=nullptr) aux2=aux2->seg;
                aux2->seg=aux;
            }
            posicio=posicio->seg;
            }
        }
    }
    return *this;
}

template <typename Valor>
cataleg<Valor>::~cataleg() noexcept {
    for (nat i=0; i<_M; i++) {
        node_hash *actual=_taula[i];
        while (actual!=nullptr) {
            node_hash* segu=actual->seg;
            delete actual;
            actual=segu;
        }
    }
    delete[] _taula;
}

template <typename Valor>
void cataleg<Valor>::assig(const string &k, const Valor &v) {
    if (k=="") throw error(ClauStringBuit);
    unsigned long i = util::hash(k);
    nat bucket=i%_M;
    node_hash *actual=_taula[bucket];
    if (existeix(k)) {
        while (actual->clau!=k) actual=actual->seg;
        actual->valor=v;
    }
    else {
        node_hash *nou=new node_hash{k, v, _taula[bucket]};
        _taula[bucket]=nou;
        _quants++;
    }
}

template <typename Valor>
void cataleg<Valor>::elimina(const string &k) {
    if (existeix(k)==false) throw error(ClauInexistent);
    else {
        unsigned long i = util::hash(k);
        nat bucket=i%_M;
        node_hash *actual=_taula[bucket];
        node_hash *aux=nullptr;
        while (actual->clau!=k) {
            aux=actual;
            actual=actual->seg;
        }
        if (actual==_taula[bucket]) {
            _taula[bucket]=actual->seg;
            delete actual;
            _quants--;
        }
        else {
            aux->seg=actual->seg;
            delete actual;
            _quants--;
        }
    }
}

template <typename Valor>
bool cataleg<Valor>::existeix(const string &k) const noexcept {
    unsigned long i = util::hash(k);
    nat bucket=i%_M;
    node_hash *actual=_taula[bucket];
    bool trobat=false;
    while (actual!=nullptr and not trobat) {
        if (actual->clau == k) {
            trobat=true;
        }
        actual=actual->seg;
    }
    return trobat;
}

template <typename Valor>
Valor cataleg<Valor>::operator[](const string &k) const {
    if (existeix(k)==false) throw error(ClauInexistent);
    unsigned long i = util::hash(k);
    nat bucket=i%_M;
    node_hash *actual=_taula[bucket];
    while (actual->clau!=k) actual=actual->seg;
    return actual->valor;
}

template <typename Valor>
nat cataleg<Valor>::quants() const noexcept {
    return _quants;
}



