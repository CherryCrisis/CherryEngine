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

public class Rigidbody : Behaviour {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  public Rigidbody(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.Rigidbody_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Rigidbody obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_Rigidbody(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public PhysicActor m_physicActor {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Rigidbody_m_physicActor_get(swigCPtr);
      PhysicActor ret = (cPtr == global::System.IntPtr.Zero) ? null : new PhysicActor(cPtr, false);
      return ret;
    } 
  }

  public void SetEnabled(bool isEnabled) {
    CherryEnginePINVOKE.Rigidbody_SetEnabled(swigCPtr, isEnabled);
  }

  public bool GetEnabled() {
    bool ret = CherryEnginePINVOKE.Rigidbody_GetEnabled(swigCPtr);
    return ret;
  }

  public void SetKinematic(bool isKinematic) {
    CherryEnginePINVOKE.Rigidbody_SetKinematic(swigCPtr, isKinematic);
  }

  public bool GetKinematic() {
    bool ret = CherryEnginePINVOKE.Rigidbody_GetKinematic(swigCPtr);
    return ret;
  }

  public void SetGravity(bool useGravity) {
    CherryEnginePINVOKE.Rigidbody_SetGravity(swigCPtr, useGravity);
  }

  public bool GetGravity() {
    bool ret = CherryEnginePINVOKE.Rigidbody_GetGravity(swigCPtr);
    return ret;
  }

  public void SetDensity(float density) {
    CherryEnginePINVOKE.Rigidbody_SetDensity(swigCPtr, density);
  }

  public float GetDensity() {
    float ret = CherryEnginePINVOKE.Rigidbody_GetDensity(swigCPtr);
    return ret;
  }

  public void SetMaxVel(float velocity) {
    CherryEnginePINVOKE.Rigidbody_SetMaxVel(swigCPtr, velocity);
  }

  public float GetMaxVel() {
    float ret = CherryEnginePINVOKE.Rigidbody_GetMaxVel(swigCPtr);
    return ret;
  }

  public void SetMaxAngVel(float velocity) {
    CherryEnginePINVOKE.Rigidbody_SetMaxAngVel(swigCPtr, velocity);
  }

  public float GetMaxAngVel() {
    float ret = CherryEnginePINVOKE.Rigidbody_GetMaxAngVel(swigCPtr);
    return ret;
  }

  public void SetMaxDepVel(float velocity) {
    CherryEnginePINVOKE.Rigidbody_SetMaxDepVel(swigCPtr, velocity);
  }

  public float GetMaxDepVel() {
    float ret = CherryEnginePINVOKE.Rigidbody_GetMaxDepVel(swigCPtr);
    return ret;
  }

  public void SetVelocity(Vector3 vel) {
    CherryEnginePINVOKE.Rigidbody_SetVelocity(swigCPtr, Vector3.getCPtr(vel));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddForce(Vector3 force, EForceMode mode) {
    CherryEnginePINVOKE.Rigidbody_AddForce(swigCPtr, Vector3.getCPtr(force), (int)mode);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddTorque(Vector3 force, EForceMode mode) {
    CherryEnginePINVOKE.Rigidbody_AddTorque(swigCPtr, Vector3.getCPtr(force), (int)mode);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

}

}