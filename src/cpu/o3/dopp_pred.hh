// Akk[DOPP2]
#ifndef __CPU_O3_DOPP_PRED_HH__
#define __CPU_O3_DOPP_PRED_HH__

#include <vector>

#include "cpu/o3/lsq_unit.hh"
#include "cpu/inst_seq.hh"
#include "mem/port.hh"
#include "sim/sim_object.hh"

using std::max, std::min;

template <class Impl>
class LastTimeAddressPredictor {
    class LastTimeAddressPredictorElem {
        public:
        size_t lru;
        Addr pc;
        Addr addr;
        LastTimeAddressPredictorElem(Addr _pc = 0, Addr _addr = 0, size_t _lru = 0): addr{_addr}, lru{_lru}, pc{_pc} {} 
    };
    
    size_t size;
    std::vector<LastTimeAddressPredictorElem> elements;

public:

    LastTimeAddressPredictor(size_t _size): size{_size}, elements(_size){
        for (int i = 0; i < size; i++){
            elements.at(i).lru = i;
        }
    }

    Addr get(Addr pc){
        Addr pred_addr = 0;
        size_t hit_lru = size;
        for (LastTimeAddressPredictorElem& elem: elements){
            if (elem.pc == pc){
                hit_lru = elem.lru;
                elem.lru = 0;
                pred_addr = elem.addr;
                break;
            }
        }
        if (hit_lru == size){
            return 0;
        }
        for (LastTimeAddressPredictorElem& elem: elements){
            if (elem.pc == pc){
                continue;
            }
            else if (elem.lru <= hit_lru){
                elem.lru++;
            }
        }
        return pred_addr;
    }

    void train(Addr pc, Addr addr){
        size_t max_lru_index = 0;
        for (size_t i = 0; i < size; i++){
            if (elements.at(i).lru == size - 1){
                max_lru_index = i;
            }
            if (elements.at(i).pc == pc){
                elements.at(i).addr = addr;
                return; 
            }
        }
        for (size_t i = 0; i < size; i++){
            elements.at(i).lru++;
        }
        elements.at(max_lru_index).lru = 0;
        elements.at(max_lru_index).pc = pc;
        elements.at(max_lru_index).addr = addr;
    }
};

template <class Impl>
class IPStridePredictor {
    class IPStridePredictorElem {
        public:
        Addr ip = 0;
        Addr addr = 0;
        int64_t stride = 0;
        size_t lru = 0;
        int8_t conf = -1;

        IPStridePredictorElem(Addr _ip = 0, Addr _addr = 0, int64_t _stride = 0, size_t _lru = 0, int8_t _conf = -1): ip{_ip}, addr{_addr}, stride{_stride}, lru{_lru}, conf{_conf} {}
    };
public:
    const int8_t MAX_CONF = 2;
    size_t size;
    std::vector<IPStridePredictorElem> elements;

    IPStridePredictor(size_t _size): size{_size}, elements(_size){
        for (int i = 0; i < size; i++){
            elements.at(i).lru = i;
        }
    }

    Addr get(Addr ip) {
        for (IPStridePredictorElem& elem: elements){
            if (elem.ip == ip){
                if (elem.conf < MAX_CONF - 1) break;
                return elem.addr + elem.stride;
            }
        }
        return 0;
    }

    void train(Addr ip, Addr addr){
        int addr_index = -1, lru_index = 0;
        for (int i = 0; i < size; i++){
            if (elements[i].ip == ip){
                addr_index = i;
                break;
            }
            lru_index = (elements[i].lru > elements[lru_index].lru) ? i : lru_index;
        }
        if (addr_index == -1){
            // ip not present in the table 
            IPStridePredictorElem& elem = elements[lru_index];
            
            addr_index = lru_index;
            
            elem.ip = ip;
            elem.stride = 0;
            elem.addr = addr;
            elem.conf = -1; // this will be increased to 0 this later
        }
        IPStridePredictorElem& elem = elements[addr_index];
        for (int i = 0; i < size; i++){
            assert((i == addr_index) || (elem.lru != elements[i].lru));
            if (elem.lru > elements[i].lru){
                elements[i].lru++;
            }
        }
        elem.lru = 0;
        
        int64_t stride = ((int64_t)addr - elem.addr);

        elem.addr = addr;

        if (stride == elem.stride){
            elem.conf = min(elem.conf + 1, (int)MAX_CONF);
        }
        else{
            elem.conf = max(0, elem.conf - 1);
            if(elem.conf == 0){
                elem.stride = stride;
            }
        }
    }
};

#endif