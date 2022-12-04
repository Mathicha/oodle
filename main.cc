#include <napi.h>
#include "oodle2net.h"

typedef Napi::Buffer<uint8_t> Buffer;

struct OodleNetwork1_Compressor
{
    void *dic;
    OodleNetwork1UDP_State *state;
    OodleNetwork1_Shared *shared;
    bool decode(const void *comp, OO_SINTa compLen, void *raw, OO_SINTa rawLen)
    {
        return OodleNetwork1UDP_Decode(state, shared, comp, compLen, raw, rawLen);
    }
};

class Oodle : public Napi::ObjectWrap<Oodle>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        auto func = DefineClass(env, "Oodle", {InstanceMethod<&Oodle::Decode>("decode")});

        auto *constructor = new Napi::FunctionReference();

        *constructor = Napi::Persistent(func);
        exports.Set("Oodle", func);

        env.SetInstanceData(constructor);
        return exports;
    }
    Oodle(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Oodle>(info)
    {
        auto env = info.Env();

        if (info.Length() != 1 || !info[0].IsBuffer())
        {
            Napi::Error::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return;
        }

        auto *data = info[0].As<Buffer>().Data();

        auto magic = *(reinterpret_cast<OO_U32 *>(&data[0]));
        if (magic != 0x11235801)
        {
            Napi::Error::New(env, "Wrong magic number").ThrowAsJavaScriptException();
            return;
        }
        auto compressor = *(reinterpret_cast<OO_U32 *>(&data[4]));
        auto ht_bits = *(reinterpret_cast<OO_U32 *>(&data[8]));
        auto dic_size = *(reinterpret_cast<OO_U32 *>(&data[12]));
        auto oodle_major_version = *(reinterpret_cast<OO_U32 *>(&data[16]));
        auto dic_complen = *(reinterpret_cast<OO_U32 *>(&data[20]));
        auto statecompacted_size = *(reinterpret_cast<OO_U32 *>(&data[24]));
        auto statecompacted_complen = *(reinterpret_cast<OO_U32 *>(&data[28]));

        pCompressor = {0};
        pCompressor.dic = malloc(dic_size);
        auto *on1udpnew_compacted = (OodleNetwork1UDP_StateCompacted *)malloc((size_t)statecompacted_size);
        memcpy(pCompressor.dic, &data[32], dic_size);
        memcpy(on1udpnew_compacted, &data[32 + dic_size], statecompacted_size);

        auto on1udpnew_state_size = OodleNetwork1UDP_State_Size();
        pCompressor.state = (OodleNetwork1UDP_State *)malloc(on1udpnew_state_size);
        if (!OodleNetwork1UDP_State_Uncompact_ForVersion(pCompressor.state, on1udpnew_compacted, oodle_major_version))
        {
            Napi::Error::New(env, "Failed to uncompact state").ThrowAsJavaScriptException();
            return;
        }
        free(on1udpnew_compacted);

        auto shared_size = OodleNetwork1_Shared_Size(ht_bits);
        pCompressor.shared = (OodleNetwork1_Shared *)malloc(shared_size);
        OodleNetwork1_Shared_SetWindow(pCompressor.shared, ht_bits, pCompressor.dic, (OO_S32)dic_size);
    }

private:
    OodleNetwork1_Compressor pCompressor;
    Napi::Value Decode(const Napi::CallbackInfo &info)
    {
        auto env = info.Env();

        if (info.Length() != 2 || !info[0].IsBuffer() || !info[1].IsBuffer())
        {
            Napi::Error::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        }

        auto comp = info[0].As<Buffer>();
        auto raw = info[1].As<Buffer>();

        if (!pCompressor.decode((const void *)comp.Data(), (OO_SINTa)comp.Length(), (void *)raw.Data(), (OO_SINTa)raw.Length()))
        {
            Napi::Error::New(env, "Failed to decode").ThrowAsJavaScriptException();
        }

        return raw;
    }
};

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Oodle::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)