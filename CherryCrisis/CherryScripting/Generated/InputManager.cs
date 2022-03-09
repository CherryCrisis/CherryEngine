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

public class InputManager : SingletonInput {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal InputManager(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.InputManager_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(InputManager obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_InputManager(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public bool GetKey(Keycode key) {
    bool ret = CherryEnginePINVOKE.InputManager_GetKey(swigCPtr, (int)key);
    return ret;
  }

  public bool GetKeyDown(Keycode key) {
    bool ret = CherryEnginePINVOKE.InputManager_GetKeyDown(swigCPtr, (int)key);
    return ret;
  }

  public bool GetKeyUp(Keycode key) {
    bool ret = CherryEnginePINVOKE.InputManager_GetKeyUp(swigCPtr, (int)key);
    return ret;
  }

  public float GetAxis(string axisName) {
    float ret = CherryEnginePINVOKE.InputManager_GetAxis(swigCPtr, axisName);
    return ret;
  }

  public InputManager() : this(CherryEnginePINVOKE.new_InputManager(), true) {
  }

}

}