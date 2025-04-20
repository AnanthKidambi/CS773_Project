// Akk[DOPP2]
#ifndef __CPU_O3_DOPP_PRED_HH__
#define __CPU_O3_DOPP_PRED_HH__

#include <vector>
#include <string>

#include "cpu/o3/lsq_unit.hh"
#include "cpu/inst_seq.hh"
#include "mem/port.hh"
#include "sim/sim_object.hh"

using std::max, std::min;

template <class Impl>
class DOPPPredictor{
public:
    virtual Addr get(Addr pc) = 0;
    virtual void train(Addr pc, Addr addr) = 0;
};

template <class Impl>
class LastTimeAddressPredictor : public DOPPPredictor<Impl> {
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

    virtual Addr get (Addr pc) override {
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

    virtual void train (Addr pc, Addr addr) override {
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
class IPStridePredictor : public DOPPPredictor<Impl> {
    static constexpr uint8_t PC_ACCURACY_THRESH_PERCENT = 0;
    class IPStridePredictorElem {
    public:
        Addr ip = 0;
        Addr addr = 0;
        int64_t stride = 0;
        size_t lru = 0;
        int8_t conf = -1;
        uint64_t correct_preds = 1;
        uint64_t total_preds = 1;

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

    virtual Addr get (Addr ip) override {
        for (IPStridePredictorElem& elem: elements){
            if (elem.ip == ip){
                if (elem.conf < MAX_CONF - 1) break;
                if (elem.total_preds * PC_ACCURACY_THRESH_PERCENT > elem.correct_preds * 100) break;
                return elem.addr + elem.stride;
            }
        }
        return 0;
    }

    virtual void train(Addr ip, Addr addr) override {
        int addr_index = -1, max_lru_index = 0;
        for (int i = 0; i < size; i++){
            if (elements[i].ip == ip){
                addr_index = i;
                break;
            }
            max_lru_index = (elements[i].lru > elements[max_lru_index].lru) ? i : max_lru_index;
        }
        if (addr_index == -1){
            // ip not present in the table 
            IPStridePredictorElem& elem = elements[max_lru_index];
            
            addr_index = max_lru_index;
            
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
            elem.correct_preds++;
        }
        else{
            elem.conf = max(0, elem.conf - 1);
            if(elem.conf == 0){
                elem.stride = stride;
            }
        }
        elem.total_preds++;
    }
};

template <class Impl>
class ComplexStridePredictor : public DOPPPredictor<Impl> {
    static constexpr uint8_t NUM_SIGN_BITS = 15;
    static constexpr uint8_t CONF_MAX = 3;
    class CSPT {
    public:
        int stride;
        int conf;

        CSPT(): stride{0}, conf{0} {}
    };

    class IP_TABLE {
    private:
        uint32_t signature; // CPLX signature
    public:
        Addr ip;
        uint16_t ip_valid; // Valid IP or not
        int64_t addr;
        int lru;

        IP_TABLE() {
            ip = 0;
            ip_valid = 0;
            signature = 0;
            addr = 0;
            lru = 0;
        }

        uint32_t get_signature() {
            return signature & ((1 << NUM_SIGN_BITS) - 1);
        }
        void set_signature(int sign) {
            signature = sign & ((1 << NUM_SIGN_BITS) - 1);
        }
        static int sign_fix(int sign) {
            return sign & ((1 << NUM_SIGN_BITS) - 1);
        }
    };

    std::vector<IP_TABLE> trackers_cplx;
    std::vector<CSPT> cspt;
public:
    ComplexStridePredictor(size_t _size): trackers_cplx(_size), cspt(1 << NUM_SIGN_BITS) {
        for (int i = 0; i < _size; i++){
            trackers_cplx[i].lru = i;
        }
    }

    virtual void train(Addr ip, Addr addr) override {
        int addr_index = -1, max_lru_index = 0;
        for (int i = 0; i < trackers_cplx.size(); i++){
            if ((trackers_cplx.at(i).ip == ip) && trackers_cplx.at(i).ip_valid){
                addr_index = i;
                break;
            }
            max_lru_index = (trackers_cplx.at(i).lru > trackers_cplx.at(max_lru_index).lru) ? i : max_lru_index;
        }
        if (addr_index == -1){
            // ip not present in the table 
            IP_TABLE& tracker = trackers_cplx[max_lru_index];
            assert(tracker.lru == trackers_cplx.size() - 1);
            
            addr_index = max_lru_index;
            
            tracker.ip = ip;

            tracker.set_signature(0);
            tracker.ip_valid = 1;
            tracker.addr = addr;
        }
        IP_TABLE& tracker = trackers_cplx[addr_index];
        for (int i = 0; i < trackers_cplx.size(); i++){
            assert((i == addr_index) || (tracker.lru != trackers_cplx[i].lru));
            if (tracker.lru > trackers_cplx[i].lru){
                trackers_cplx[i].lru++;
            }
        }
        tracker.lru = 0;
        int64_t stride = ((int64_t)addr - tracker.addr);
        
        tracker.addr = addr;

        int& pred_stride = cspt[tracker.get_signature()].stride;
        int& conf = cspt[tracker.get_signature()].conf;

        if(pred_stride == stride){
            conf = min(conf+1, (int)CONF_MAX);
        }
        else{
            conf = max(conf-1, 0);
            if(conf == 0){
                pred_stride = stride;
            }
        }
        int sign = (tracker.get_signature() << 1) ^ pred_stride;
        tracker.set_signature(sign);
    }
    
    virtual Addr get(Addr ip) override {
        for (IP_TABLE& tracker: trackers_cplx){
            if (tracker.ip == ip){           
                int sign = tracker.get_signature();
                int conf = cspt[sign].conf;
                int stride = cspt[sign].stride;
                if(conf >= CONF_MAX - 1){
                    return (int64_t)tracker.addr + stride;
                }
            }
        }
        return 0;
    }

};

template <class Impl>
DOPPPredictor<Impl>* createDOPPPredictor(size_t _size, uint8_t predictor_type){
    if (predictor_type == 0){
        return new LastTimeAddressPredictor<Impl>(_size);
    }
    else if (predictor_type == 1){
        return new IPStridePredictor<Impl>(_size);
    }
    else if (predictor_type == 2){
        return new ComplexStridePredictor<Impl>(_size);
    }
    else{
        fatal("Unknown predictor type %d\n", predictor_type);
    }
}

#endif