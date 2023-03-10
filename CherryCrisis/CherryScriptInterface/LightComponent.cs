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

public class LightComponent : Behaviour {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  public LightComponent(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.LightComponent_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(LightComponent obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_LightComponent(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public Transform m_transform {
    set {
      CherryEnginePINVOKE.LightComponent_m_transform_set(swigCPtr, Transform.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.LightComponent_m_transform_get(swigCPtr);
      Transform ret = (cPtr == global::System.IntPtr.Zero) ? null : new Transform(cPtr, false);
      return ret;
    } 
  }

  public void SetPosition(Vector3 newPosition) {
    CherryEnginePINVOKE.LightComponent_SetPosition(swigCPtr, Vector3.getCPtr(newPosition));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRotation(Vector3 newDirection) {
    CherryEnginePINVOKE.LightComponent_SetRotation(swigCPtr, Vector3.getCPtr(newDirection));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetAmbient(Vector3 newAmbient) {
    CherryEnginePINVOKE.LightComponent_SetAmbient(swigCPtr, Vector3.getCPtr(newAmbient));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetDiffuse(Vector3 newDiffuse) {
    CherryEnginePINVOKE.LightComponent_SetDiffuse(swigCPtr, Vector3.getCPtr(newDiffuse));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetSpecular(Vector3 newSpecular) {
    CherryEnginePINVOKE.LightComponent_SetSpecular(swigCPtr, Vector3.getCPtr(newSpecular));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetAttenuation(Vector3 newAttenuation) {
    CherryEnginePINVOKE.LightComponent_SetAttenuation(swigCPtr, Vector3.getCPtr(newAttenuation));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetCutoff(float newCutoff) {
    CherryEnginePINVOKE.LightComponent_SetCutoff(swigCPtr, newCutoff);
  }

  public void SetOuterCutoff(float newOuterCutoff) {
    CherryEnginePINVOKE.LightComponent_SetOuterCutoff(swigCPtr, newOuterCutoff);
  }

  public void SetLightType(ELightType newType) {
    CherryEnginePINVOKE.LightComponent_SetLightType(swigCPtr, (int)newType);
  }

  public Vector3 GetPosition() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetPosition(swigCPtr), true);
    return ret;
  }

  public Vector3 GetDirection() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetDirection(swigCPtr), true);
    return ret;
  }

  public Vector3 GetAmbient() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetAmbient(swigCPtr), true);
    return ret;
  }

  public Vector3 GetDiffuse() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetDiffuse(swigCPtr), true);
    return ret;
  }

  public Vector3 GetSpecular() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetSpecular(swigCPtr), true);
    return ret;
  }

  public Vector3 GetAttenuation() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.LightComponent_GetAttenuation(swigCPtr), true);
    return ret;
  }

  public float GetCutoff() {
    float ret = CherryEnginePINVOKE.LightComponent_GetCutoff(swigCPtr);
    return ret;
  }

  public float GetOuterCutoff() {
    float ret = CherryEnginePINVOKE.LightComponent_GetOuterCutoff(swigCPtr);
    return ret;
  }

  public ELightType GetLightType() {
    ELightType ret = (ELightType)CherryEnginePINVOKE.LightComponent_GetLightType(swigCPtr);
    return ret;
  }

}

}
