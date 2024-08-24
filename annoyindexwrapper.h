#ifndef ANNOYINDEXWRAPPER_H
#define ANNOYINDEXWRAPPER_H

#include <napi.h>
#include "annoylib.h"

class AnnoyIndexWrapper : public Napi::ObjectWrap<AnnoyIndexWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    int getDimensions() const;
    AnnoyIndexInterface<int, float> *annoyIndex;

    explicit AnnoyIndexWrapper(const Napi::CallbackInfo &callbackInfo);
    virtual ~AnnoyIndexWrapper();


private:

    static Napi::FunctionReference *constructor;

    Napi::Object AddItem(const Napi::CallbackInfo &info);
    Napi::Object Build(const Napi::CallbackInfo &info) const;
    Napi::Object Save(const Napi::CallbackInfo &info) const;
    Napi::Object Load(const Napi::CallbackInfo &info) const;
    Napi::Object Unload(const Napi::CallbackInfo &info) const;
    Napi::Object GetItem(const Napi::CallbackInfo &info) const;
    Napi::Object GetNNSByVector(const Napi::CallbackInfo &info);
    Napi::Object GetNNSByItem(const Napi::CallbackInfo &info);
    Napi::Object GetNItems(const Napi::CallbackInfo &info) const;
    Napi::Object GetDistance(const Napi::CallbackInfo &info) const;



    bool getFloatArrayParam(const Napi::CallbackInfo &info,
                            int paramIndex, float *vec);

    Napi::Object setNNReturnValues(
            int numberOfNeighbors, bool includeDistances,
            const std::vector<int> &nnIndexes, const std::vector<float> &distances,
            const Napi::CallbackInfo &info);

    void getSupplementaryGetNNsParams(
            const Napi::CallbackInfo &info,
            int &numberOfNeighbors, int &searchK, bool &includeDistances);

    int annoyDimensions;
};

#endif
