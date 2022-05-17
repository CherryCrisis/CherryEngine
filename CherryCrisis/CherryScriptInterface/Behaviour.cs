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

public class Behaviour : Object {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  public Behaviour(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.Behaviour_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Behaviour obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_Behaviour(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public Entity GetHost() {
    Entity ret = new Entity(CherryEnginePINVOKE.Behaviour_GetHost(swigCPtr), false);
    return ret;
  }

	public Entity host { get => GetHost(); }

	public override string ToString() => System.String.Format("{0} ({1})", base.ToString(), host);

	public Behaviour GetComponent(System.Type type) => host.GetComponent(type);
	public T GetComponent<T>() where T : Behaviour => host.GetComponent<T>();

	public Behaviour AddComponent(System.Type type) => host.AddComponent(type);
	public T AddComponent<T>() where T : Behaviour => host.AddComponent<T>();
	
  public Behaviour() : this(CherryEnginePINVOKE.new_Behaviour(), true) {
    SwigDirectorConnect();
  }

  private void SwigDirectorConnect() {
    CherryEnginePINVOKE.Behaviour_director_connect(swigCPtr);
  }
}

}