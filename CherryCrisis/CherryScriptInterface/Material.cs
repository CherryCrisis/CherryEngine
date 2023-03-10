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

public class Material : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  private bool swigCMemOwnBase;

  internal Material(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwnBase = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Material obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Material() {
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
          CherryEnginePINVOKE.delete_Material(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public Vector3 m_ambient {
    set {
      CherryEnginePINVOKE.Material_m_ambient_set(swigCPtr, Vector3.getCPtr(value));
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Material_m_ambient_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public Vector3 m_diffuse {
    set {
      CherryEnginePINVOKE.Material_m_diffuse_set(swigCPtr, Vector3.getCPtr(value));
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Material_m_diffuse_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public Vector3 m_specular {
    set {
      CherryEnginePINVOKE.Material_m_specular_set(swigCPtr, Vector3.getCPtr(value));
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Material_m_specular_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public Vector3 m_emissive {
    set {
      CherryEnginePINVOKE.Material_m_emissive_set(swigCPtr, Vector3.getCPtr(value));
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Material_m_emissive_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_shininess {
    set {
      CherryEnginePINVOKE.Material_m_shininess_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_shininess_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public bool m_hasNormal {
    set {
      CherryEnginePINVOKE.Material_m_hasNormal_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      bool ret = CherryEnginePINVOKE.Material_m_hasNormal_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_specularFactor {
    set {
      CherryEnginePINVOKE.Material_m_specularFactor_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_specularFactor_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_metallicFactor {
    set {
      CherryEnginePINVOKE.Material_m_metallicFactor_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_metallicFactor_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_roughnessFactor {
    set {
      CherryEnginePINVOKE.Material_m_roughnessFactor_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_roughnessFactor_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_ao {
    set {
      CherryEnginePINVOKE.Material_m_ao_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_ao_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_clearCoatFactor {
    set {
      CherryEnginePINVOKE.Material_m_clearCoatFactor_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_clearCoatFactor_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public float m_clearCoatRoughnessFactor {
    set {
      CherryEnginePINVOKE.Material_m_clearCoatRoughnessFactor_set(swigCPtr, value);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      float ret = CherryEnginePINVOKE.Material_m_clearCoatRoughnessFactor_get(swigCPtr);
      if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public void SetTexture(ETextureType type, Texture newTexture) {
    CherryEnginePINVOKE.Material_SetTexture__SWIG_0(swigCPtr, (int)type, Texture.getCPtr(newTexture));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetTexture(ETextureType type, string filepath) {
    CherryEnginePINVOKE.Material_SetTexture__SWIG_1(swigCPtr, (int)type, filepath);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

}

}
