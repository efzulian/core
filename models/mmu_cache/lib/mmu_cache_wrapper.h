#ifndef _SCGENMOD_mmu_cache_wrapper_
#define _SCGENMOD_mmu_cache_wrapper_

#include "systemc.h"

struct cctrltype {
    sc_logic burst;
    sc_logic dfrz;
    sc_logic ifrz;
    sc_logic dsnoop;
    sc_lv<2> dcs;
    sc_lv<2> ics;
};

inline ostream& operator<<(ostream& os, const cctrltype& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const cctrltype&, const std::string &) {}

inline int operator== (const cctrltype& left, const cctrltype& right) {
    return 0;
}

/************************************/

struct icdiag_in_type {
    sc_lv<32> addr;
    sc_logic enable;
    sc_logic read;
    sc_logic tag;
    sc_logic ctx;
    sc_logic flush;
    sc_logic ilramen;
    cctrltype cctrl;
    sc_logic pflush;
    sc_lv<20> pflushaddr;
    sc_logic pflushtyp;
    sc_lv<4> ilock;
    sc_logic scanen;
};

inline ostream& operator<<(ostream& os, const icdiag_in_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const icdiag_in_type&, const std::string &) {}

inline int operator== (const icdiag_in_type& left, const icdiag_in_type& right) {
    return 0;
}

/************************************/

struct icache_in_type {
    sc_lv<32> rpc;
    sc_lv<32> fpc;
    sc_lv<32> dpc;
    sc_logic rbranch;
    sc_logic fbranch;
    sc_logic inull;
    sc_logic su;
    sc_logic flush;
    sc_logic flushl;
    sc_lv<29> fline;
    sc_logic pnull;
};

inline ostream& operator<<(ostream& os, const icache_in_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const icache_in_type&, const std::string &) {}

inline int operator== (const icache_in_type& left, const icache_in_type& right) {
    return 0;
}

/************************************/

struct icache_out_type {
    sc_lv<32> data[4];
    sc_lv<2> set;
    sc_logic mexc;
    sc_logic hold;
    sc_logic flush;
    sc_logic diagrdy;
    sc_lv<32> diagdata;
    sc_logic mds;
    sc_lv<32> cfg;
    sc_logic idle;
};

inline ostream& operator<<(ostream& os, const icache_out_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const icache_out_type&, const std::string &) {}

inline int operator== (const icache_out_type& left, const icache_out_type& right) {
    return 0;
}

/************************************/

struct dcache_in_type {
    sc_lv<8> asi;
    sc_lv<32> maddress;
    sc_lv<32> eaddress;
    sc_lv<32> edata;
    sc_lv<2> size;
    sc_logic enaddr;
    sc_logic eenaddr;
    sc_logic nullify;
    sc_logic lock;
    sc_logic read;
    sc_logic write;
    sc_logic flush;
    sc_logic flushl;
    sc_logic dsuen;
    sc_logic msu;
    sc_logic esu;
    sc_logic intack;
};

inline ostream& operator<<(ostream& os, const dcache_in_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const dcache_in_type&, const std::string &) {}

inline int operator== (const dcache_in_type& left, const dcache_in_type& right) {
    return 0;
}

/************************************/

struct dcache_out_type {
    sc_lv<32> data[4];
    sc_lv<2> set;
    sc_logic mexc;
    sc_logic hold;
    sc_logic mds;
    sc_logic werr;
    icdiag_in_type icdiag;
    sc_logic cache;
    sc_logic idle;
    sc_logic scanen;
    sc_logic testen;
};

inline ostream& operator<<(ostream& os, const dcache_out_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const dcache_out_type&, const std::string &) {}

inline int operator== (const dcache_out_type& left, const dcache_out_type& right) {
    return 0;
}


/************************************/

#ifndef _AHB_MST_IN_TYPE_
#define _AHB_MST_IN_TYPE_

struct ahb_mst_in_type {
    sc_lv<16> hgrant;
    sc_logic hready;
    sc_lv<2> hresp;
    sc_lv<32> hrdata;
    sc_logic hcache;
    sc_lv<32> hirq;
    sc_logic testen;
    sc_logic testrst;
    sc_logic scanen;
    sc_logic testoen;
};

inline ostream& operator<<(ostream& os, const ahb_mst_in_type& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const ahb_mst_in_type&, const std::string &) {}

inline int operator== (const ahb_mst_in_type& left, const ahb_mst_in_type& right) {
    return 0;
}

#endif

/************************************/

#ifndef _AHB_MST_OUT_TYPE_ADAPTER_
#define _AHB_MST_OUT_TYPE_ADAPTER_

struct ahb_mst_out_type_adapter {
    sc_logic hbusreq;
    sc_logic hlock;
    sc_lv<2> htrans;
    sc_lv<32> haddr;
    sc_logic hwrite;
    sc_lv<3> hsize;
    sc_lv<3> hburst;
    sc_lv<4> hprot;
    sc_lv<32> hwdata;
    sc_lv<32> hirq;
    sc_lv<32> hconfig[8];
    int hindex;
};

inline ostream& operator<<(ostream& os, const ahb_mst_out_type_adapter& a) {
    return os;
}

inline void sc_trace(sc_trace_file *, const ahb_mst_out_type_adapter&, const std::string &) {}

inline int operator== (const ahb_mst_out_type_adapter& left, const ahb_mst_out_type_adapter& right) {
    return 0;
}

#endif

/************************************/

class mmu_cache_wrapper : public sc_foreign_module
{
public:
    sc_in<sc_logic> rst;
    sc_in<sc_logic> clk;
    sc_in<icache_in_type> ici;
    sc_out<icache_out_type> ico;
    sc_in<dcache_in_type> dci;
    sc_out<dcache_out_type> dco;
    sc_in<ahb_mst_in_type> ahbi;
    sc_out<ahb_mst_out_type_adapter> ahbo;
    sc_in<sc_logic> fpuholdn;
    sc_in<sc_logic> hclk;
    sc_in<sc_logic> sclk;
    sc_in<sc_logic> hclken;


    mmu_cache_wrapper(sc_module_name nm, const char* hdl_name)
     : sc_foreign_module(nm),
       rst("rst"),
       clk("clk"),
       ici("ici"),
       ico("ico"),
       dci("dci"),
       dco("dco"),
       ahbi("ahbi"),
       ahbo("ahbo"),
       fpuholdn("fpuholdn"),
       hclk("hclk"),
       sclk("sclk"),
       hclken("hclken")
    {
        elaborate_foreign_module(hdl_name);
    }
    ~mmu_cache_wrapper()
    {}

};

#endif

