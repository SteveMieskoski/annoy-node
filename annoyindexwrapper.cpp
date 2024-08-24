#include "annoyindexwrapper.h"
#include "kissrandom.h"
#include <vector>

using namespace Napi;

Napi::FunctionReference *AnnoyIndexWrapper::constructor;

Napi::Object AnnoyIndexWrapper::Init(Napi::Env env, Napi::Object exports) {

    Napi::Function ctor = DefineClass(env, "Annoy", {
            InstanceMethod("addItem", reinterpret_cast<InstanceMethodCallback>(&AnnoyIndexWrapper::AddItem)),
            InstanceMethod("build", (InstanceMethodCallback) &AnnoyIndexWrapper::Build),
            InstanceMethod("save", (InstanceMethodCallback) &AnnoyIndexWrapper::Save),
            InstanceMethod("load", (InstanceMethodCallback) &AnnoyIndexWrapper::Load),
            InstanceMethod("unload", (InstanceMethodCallback) &AnnoyIndexWrapper::Unload),
            InstanceMethod("getNItems", (InstanceMethodCallback) &AnnoyIndexWrapper::GetNItems),
            InstanceMethod("getDistance", (InstanceMethodCallback) &AnnoyIndexWrapper::GetDistance),
            InstanceMethod("getItem", (InstanceMethodCallback) &AnnoyIndexWrapper::GetItem),
            InstanceMethod("getNNsByVector", (InstanceMethodCallback) &AnnoyIndexWrapper::GetNNSByVector),
            InstanceMethod("getNNsByItem", (InstanceMethodCallback) &AnnoyIndexWrapper::GetNNSByItem),
    } );
    constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(ctor);
    exports.Set("Annoy", ctor);
    Napi::HandleScope scope(env);

    return exports;
}

AnnoyIndexWrapper::AnnoyIndexWrapper(const CallbackInfo &callbackInfo) :
        ObjectWrap(callbackInfo), annoyDimensions(callbackInfo[0].As<Napi::Number>()) {

    Napi::String metricString = callbackInfo[1].As<Napi::String>();
    Napi::String angularStr = Napi::Value::From(callbackInfo.Env(), "Angular").ToString();
    Napi::String manhattanStr = Napi::Value::From(callbackInfo.Env(), "Manhattan").ToString();
//    metString.operator==("Angular")
    if (metricString.operator==(angularStr)) {
        annoyIndex = new AnnoyIndex<int, float, Angular, Kiss64Random>(callbackInfo[0].As<Napi::Number>());
    }
    else if (metricString.operator==(manhattanStr)) {
        annoyIndex = new AnnoyIndex<int, float, Manhattan, Kiss64Random>(callbackInfo[0].As<Napi::Number>());
    }
    else {
        annoyIndex = new AnnoyIndex<int, float, Euclidean, Kiss64Random>(callbackInfo[0].As<Napi::Number>());
    }
}

//AnnoyIndexWrapper::~AnnoyIndexWrapper() {
//    delete annoyIndex;
//}

Napi::Object AnnoyIndexWrapper::AddItem(const Napi::CallbackInfo& info) {
    int index = info[0].IsUndefined() ? 1 : info[0].As<Napi::Number>();
//    printf("index %d\n", index);
    // Get out array.
    float vec[this->getDimensions()];
    if (getFloatArrayParam(info, 1, vec)) {
//        printf("index 2nd %d\n", index);
        this->annoyIndex->add_item(index, vec);
    }

    return {};
}

Napi::Object AnnoyIndexWrapper::Build(const Napi::CallbackInfo& info) const {
    // Get out numberOfTrees.
    int numberOfTrees = info[0].IsUndefined() ? 1 : info[0].As<Napi::Number>();
    // printf("%s\n", "Calling build");
    this->annoyIndex->build(numberOfTrees);
    return {};
}

Napi::Object AnnoyIndexWrapper::Save(const Napi::CallbackInfo& info) const {
    Napi::Env env = info.Env();
    bool result = false;
    // Get out file path.
    if (!info[0].IsUndefined()) {
        std::string maybeString = info[0].As<Napi::String>().Utf8Value();
        result = this->annoyIndex->save(maybeString.c_str());
    }

    return {env, Napi::Value::From(env, result)};
}

Napi::Object AnnoyIndexWrapper::Load(const Napi::CallbackInfo& info) const {
    Napi::Env env = info.Env();
    bool result = false;
    // Get out object.

    // Get out file path.
    if (!info[0].IsUndefined()) {
        std::string maybeString = info[0].As<Napi::String>().Utf8Value();
        result = this->annoyIndex->load(maybeString.c_str());
    }
    return {env, Napi::Value::From(env, result)};
}

Napi::Object AnnoyIndexWrapper::Unload(const Napi::CallbackInfo& info) const {
    this->annoyIndex->unload();
    return {};
}

int AnnoyIndexWrapper::getDimensions() const {
    return annoyDimensions;
}

// Returns true if it was able to get items out of the array. false, if not.
bool AnnoyIndexWrapper::getFloatArrayParam(
        const Napi::CallbackInfo& info, int paramIndex, float *vec) {

    bool succeeded = false;

//    printf("paramIndex %d\n", paramIndex);
//    Napi::Value val;
    if (info[paramIndex].IsArray()) {
        // TODO: Make sure it really is OK to use Local instead of Handle here.
        Napi::Array jsArray = info[paramIndex].As<Napi::Array>();
        Napi::Value val;

        for (unsigned int i = 0; i < jsArray.Length(); i++) {
//            printf("getFloatArrayParam: i = %d\n", i);
            val = (jsArray).Get(i);
//            printf("val %s\n", val.ToString().Utf8Value().c_str());
//             printf("Adding item to array: %f\n", (float)val.As<Napi::Number>());
            vec[i] = val.ToNumber().FloatValue();
        }
        succeeded = true;
    }

    return succeeded;
}

Napi::Object AnnoyIndexWrapper::GetDistance(const Napi::CallbackInfo& info) const {
    Napi::Env env = info.Env();
    // Get out object.


    // Get out indexes.
    int indexA = info[0].IsUndefined() ? 0 : info[0].As<Napi::Number>();
    int indexB = info[1].IsUndefined() ? 0 : info[1].As<Napi::Number>();
    printf("indexA %d\n", indexA);
    printf("indexB %d\n", indexB);

    // Return the distances.

    float res = this->annoyIndex->get_distance(indexA, indexB);

//    return Napi::Number::Value::From(env, res);
    return {env, Napi::Value::From(env, res)};
}

Napi::Object AnnoyIndexWrapper::GetNItems(const Napi::CallbackInfo& info) const {
    Napi::Env env = info.Env();
    // Get out object.
    Napi::Number count = Napi::Number::New(env, this->annoyIndex->get_n_items());
    return reinterpret_cast<Object &&>(count);
}

Napi::Object AnnoyIndexWrapper::GetItem(const Napi::CallbackInfo& info) const {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

//    // Get out object.
//    AnnoyIndexWrapper* obj = ObjectWrap::Unwrap<AnnoyIndexWrapper>(info.Holder());

    // Get out index.
    int index = info[0].IsUndefined() ? 1 : info[0].As<Napi::Number>();

    // Get the vector.
    int length = this->getDimensions();
    float vec[length];
    this->annoyIndex->get_item(index, vec);

    // Allocate the return array.
    Napi::Array results = Napi::Array::New(env, length);
    for (int i = 0; i < length; ++i) {
        // printf("Adding to array: %f\n", vec[i]);
        (results).Set(i, Napi::Number::New(env, vec[i]));
    }

    return results;
}

Napi::Object AnnoyIndexWrapper::GetNNSByVector(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int numberOfNeighbors, searchK;
    bool includeDistances;
    getSupplementaryGetNNsParams(info, numberOfNeighbors, searchK, includeDistances);

    // Get out object.
    // Get out array.
    float vec[this->getDimensions()];
    if (!getFloatArrayParam(info, 0, vec)) {
        return {};
    }

    std::vector<int> nnIndexes;
    std::vector<float> distances;
    std::vector<float> *distancesPtr = nullptr;

    if (includeDistances) {
        distancesPtr = &distances;
    }

    // Make the call.
    this->annoyIndex->get_nns_by_vector(
            vec, numberOfNeighbors, searchK, &nnIndexes, distancesPtr
    );

    return setNNReturnValues(numberOfNeighbors, includeDistances, nnIndexes, distances, info);
}

Napi::Object AnnoyIndexWrapper::GetNNSByItem(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Get out object.
//    AnnoyIndexWrapper* obj = ObjectWrap::Unwrap<AnnoyIndexWrapper>(info.Holder());

    if (info[0].IsUndefined()) {
        return {};
    }

    // Get out params.
    int index = info[0].As<Napi::Number>();
    int numberOfNeighbors, searchK;
    bool includeDistances;
    getSupplementaryGetNNsParams(info, numberOfNeighbors, searchK, includeDistances);

    std::vector<int> nnIndexes;
    std::vector<float> distances;
    std::vector<float> *distancesPtr = nullptr;

    if (includeDistances) {
        distancesPtr = &distances;
    }

    // Make the call.
    this->annoyIndex->get_nns_by_item(
            index, numberOfNeighbors, searchK, &nnIndexes, distancesPtr
    );

    return setNNReturnValues(numberOfNeighbors, includeDistances, nnIndexes, distances, info);
}

void AnnoyIndexWrapper::getSupplementaryGetNNsParams(
        const Napi::CallbackInfo& info,
        int& numberOfNeighbors, int& searchK, bool& includeDistances) {

    // Get out number of neighbors.
    numberOfNeighbors = info[1].IsUndefined() ? 1 : info[1].As<Napi::Number>();

    // Get out searchK.
    searchK = info[2].IsUndefined() ? -1 : info[2].As<Napi::Number>();

    // Get out include distances flag.
    includeDistances = info[3].IsUndefined() ? false : info[3].As<Napi::Boolean>().Value();
}

Napi::Object AnnoyIndexWrapper::setNNReturnValues(
        int numberOfNeighbors, bool includeDistances,
        const std::vector<int>& nnIndexes, const std::vector<float>& distances,
        const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // Allocate the neighbors array.
    Napi::Array jsNNIndexes = Napi::Array::New(env, numberOfNeighbors);
    for (int i = 0; i < numberOfNeighbors; ++i) {
        // printf("Adding to neighbors array: %d\n", nnIndexes[i]);
        (jsNNIndexes).Set(i, Napi::Number::New(env, nnIndexes[i]));
    }

    Napi::Object jsResultObject;
    Napi::Array jsDistancesArray;

    if (includeDistances) {
        // Allocate the distances array.
        jsDistancesArray = Napi::Array::New(env, numberOfNeighbors);
        for (int i = 0; i < numberOfNeighbors; ++i) {
            // printf("Adding to distances array: %f\n", distances[i]);
            (jsDistancesArray).Set(i, Napi::Number::New(env, distances[i]));
        }

        jsResultObject = Napi::Object::New(env);
        (jsResultObject).Set(Napi::String::New(env, "neighbors"), jsNNIndexes);
        (jsResultObject).Set(Napi::String::New(env, "distances"), jsDistancesArray);
    }
    else {
        jsResultObject = jsNNIndexes.As<Napi::Object>();
    }

    return jsResultObject;
}