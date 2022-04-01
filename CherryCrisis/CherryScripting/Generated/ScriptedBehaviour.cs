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

public class ScriptedBehaviour : Behaviour {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal ScriptedBehaviour(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.ScriptedBehaviour_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ScriptedBehaviour obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_ScriptedBehaviour(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public string GetScriptPath() {
    string ret = CherryEnginePINVOKE.ScriptedBehaviour_GetScriptPath(swigCPtr);
    return ret;
  }

  public void SetScriptClass(string scriptName) {
    CherryEnginePINVOKE.ScriptedBehaviour_SetScriptClass(swigCPtr, scriptName);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

	[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
	private extern static Behaviour GetStaticInstance(System.IntPtr cPtr);

	public Behaviour GetInstance()
	{
		return GetStaticInstance(swigCPtr.Handle);
	}
	
}

}
