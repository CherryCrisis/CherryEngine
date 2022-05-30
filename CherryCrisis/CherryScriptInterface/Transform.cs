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

public class Transform : Behaviour {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  public Transform(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.Transform_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Transform obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_Transform(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public void SetPosition(Vector3 position) {
    CherryEnginePINVOKE.Transform_SetPosition(swigCPtr, Vector3.getCPtr(position));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetGlobalPosition(Vector3 position) {
    CherryEnginePINVOKE.Transform_SetGlobalPosition(swigCPtr, Vector3.getCPtr(position));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Vector3 GetPosition() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetPosition(swigCPtr), true);
    return ret;
  }

  public Vector3 GetGlobalPosition() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetGlobalPosition(swigCPtr), true);
    return ret;
  }

  public void SetRotation(Vector3 rotation) {
    CherryEnginePINVOKE.Transform_SetRotation(swigCPtr, Vector3.getCPtr(rotation));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetGlobalRotation(Vector3 rotation) {
    CherryEnginePINVOKE.Transform_SetGlobalRotation(swigCPtr, Vector3.getCPtr(rotation));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Vector3 GetRotation() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetRotation(swigCPtr), true);
    return ret;
  }

  public Vector3 GetGlobalRotation() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetGlobalRotation(swigCPtr), true);
    return ret;
  }

  public void SetScale(Vector3 scale) {
    CherryEnginePINVOKE.Transform_SetScale(swigCPtr, Vector3.getCPtr(scale));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetGlobalScale(Vector3 scale) {
    CherryEnginePINVOKE.Transform_SetGlobalScale(swigCPtr, Vector3.getCPtr(scale));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Vector3 GetScale() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetScale(swigCPtr), true);
    return ret;
  }

  public Vector3 GetGlobalScale() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_GetGlobalScale(swigCPtr), true);
    return ret;
  }

  public Vector3 Up() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_Up(swigCPtr), true);
    return ret;
  }

  public Vector3 Right() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_Right(swigCPtr), true);
    return ret;
  }

  public Vector3 Forward() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Transform_Forward(swigCPtr), true);
    return ret;
  }

		public Vector3 position { get => GetPosition(); set => SetPosition(value); }
		public Vector3 eulerAngles { get => GetRotation(); set => SetRotation(value); }
		public Vector3 scale { get => GetScale(); set => SetScale(value); }
	
}

}
