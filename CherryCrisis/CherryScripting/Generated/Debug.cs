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

public class Debug : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Debug(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Debug obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Debug() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_Debug(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public static Debug GetInstance() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Debug_GetInstance();
    Debug ret = (cPtr == global::System.IntPtr.Zero) ? null : new Debug(cPtr, false);
    return ret;
  }

  public void Log(string string_) {
    CherryEnginePINVOKE.Debug_Log(swigCPtr, string_);
  }

	public void Log(object message) => Log(message.ToString());

	public void Log(string separator, params object[] messages) => Log(string.Join(separator, messages));
	public void Log(params object[] messages) => Log(string.Join(" ; ", messages));
	
}

}