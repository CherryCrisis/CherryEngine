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

public class PhysicManager : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public PhysicManager(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(PhysicManager obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~PhysicManager() {
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
          CherryEnginePINVOKE.delete_PhysicManager(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public static PhysicManager GetInstance() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.PhysicManager_GetInstance();
    PhysicManager ret = (cPtr == global::System.IntPtr.Zero) ? null : new PhysicManager(cPtr, false);
    return ret;
  }

  public PhysicActor FindActor(Entity owningEntity) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.PhysicManager_FindActor(swigCPtr, Entity.getCPtr(owningEntity));
    PhysicActor ret = (cPtr == global::System.IntPtr.Zero) ? null : new PhysicActor(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

		public PhysicActor GetActor(Entity owningEntity) => FindActor(owningEntity);
		
  public static RaycastHit Raycast(Cell cell, Vector3 origin, Vector3 dir, float maxRange) {
    RaycastHit ret = new RaycastHit(CherryEnginePINVOKE.PhysicManager_Raycast(Cell.getCPtr(cell), Vector3.getCPtr(origin), Vector3.getCPtr(dir), maxRange), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static void AddForce(Entity entity, Vector3 force, EForceMode mode) {
    CherryEnginePINVOKE.PhysicManager_AddForce(Entity.getCPtr(entity), Vector3.getCPtr(force), (int)mode);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public PhysicManager() : this(CherryEnginePINVOKE.new_PhysicManager(), true) {
  }

}

}
