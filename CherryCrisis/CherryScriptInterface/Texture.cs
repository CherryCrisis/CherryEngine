//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace CCEngine {

public class Texture : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  private bool swigCMemOwnBase;

  internal Texture(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwnBase = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Texture obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Texture() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwnBase) {
          swigCMemOwnBase = false;
          CherryEnginePINVOKE.delete_Texture(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public int GetWidth() {
    int ret = CherryEnginePINVOKE.Texture_GetWidth(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetHeight() {
    int ret = CherryEnginePINVOKE.Texture_GetHeight(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetSize() {
    int ret = CherryEnginePINVOKE.Texture_GetSize(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetMipmapCount() {
    int ret = CherryEnginePINVOKE.Texture_GetMipmapCount(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetBlockSize() {
    int ret = CherryEnginePINVOKE.Texture_GetBlockSize(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ETextureFormat GetInternalFormat() {
    ETextureFormat ret = (ETextureFormat)CherryEnginePINVOKE.Texture_GetInternalFormat(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetInternalFormat(ETextureFormat textureFormat) {
    CherryEnginePINVOKE.Texture_SetInternalFormat(swigCPtr, (int)textureFormat);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public ETextureSurface GetSurface() {
    ETextureSurface ret = (ETextureSurface)CherryEnginePINVOKE.Texture_GetSurface(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetSurface(ETextureSurface surface) {
    CherryEnginePINVOKE.Texture_SetSurface(swigCPtr, (int)surface);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetIsFlipped() {
    bool ret = CherryEnginePINVOKE.Texture_GetIsFlipped(swigCPtr);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
