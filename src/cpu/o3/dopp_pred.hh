// Akk[DOPP2]
#ifndef __CPU_O3_DOPP_PRED_HH__
#define __CPU_O3_DOPP_PRED_HH__

#include <vector>

#include "cpu/o3/lsq_unit.hh"
#include "cpu/inst_seq.hh"
#include "mem/port.hh"
#include "sim/sim_object.hh"

template <class Impl>
class LastTimeAddressPredictor {
    class LastTimeAddressPredictorElem {
        public:
        uint8_t lru;
        Addr pc;
        Addr addr;
        LastTimeAddressPredictorElem(Addr _pc = 0, Addr _addr = 0, Addr _lru = 0): addr{_addr}, lru{_lru}, pc{_pc} {} 
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
        uint8_t hit_lru = size;
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

#endif