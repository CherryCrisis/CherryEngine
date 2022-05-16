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

public class Entity : Object {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal Entity(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.Entity_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Entity obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_Entity(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public void Destroy() {
    CherryEnginePINVOKE.Entity_Destroy(swigCPtr);
  }

  private string GetName() {
    string ret = CherryEnginePINVOKE.Entity_GetName(swigCPtr);
    return ret;
  }

  private CameraComponent GetCameraComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetCameraComponent(swigCPtr);
    CameraComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new CameraComponent(cPtr, false);
    return ret;
  }

  private CameraComponent AddCameraComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddCameraComponent(swigCPtr);
    CameraComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new CameraComponent(cPtr, false);
    return ret;
  }

  private CameraComponentVector GetAllOfCameraComponent() {
    CameraComponentVector ret = new CameraComponentVector(CherryEnginePINVOKE.Entity_GetAllOfCameraComponent(swigCPtr), true);
    return ret;
  }

  private ScriptedBehaviour GetScriptedBehaviour() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetScriptedBehaviour(swigCPtr);
    ScriptedBehaviour ret = (cPtr == global::System.IntPtr.Zero) ? null : new ScriptedBehaviour(cPtr, false);
    return ret;
  }

  private ScriptedBehaviour AddScriptedBehaviour() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddScriptedBehaviour(swigCPtr);
    ScriptedBehaviour ret = (cPtr == global::System.IntPtr.Zero) ? null : new ScriptedBehaviour(cPtr, false);
    return ret;
  }

  private ScriptedBehaviourVector GetAllOfScriptedBehaviour() {
    ScriptedBehaviourVector ret = new ScriptedBehaviourVector(CherryEnginePINVOKE.Entity_GetAllOfScriptedBehaviour(swigCPtr), true);
    return ret;
  }

  private Transform GetTransform() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetTransform(swigCPtr);
    Transform ret = (cPtr == global::System.IntPtr.Zero) ? null : new Transform(cPtr, false);
    return ret;
  }

  private Transform AddTransform() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddTransform(swigCPtr);
    Transform ret = (cPtr == global::System.IntPtr.Zero) ? null : new Transform(cPtr, false);
    return ret;
  }

  private TransformVector GetAllOfTransform() {
    TransformVector ret = new TransformVector(CherryEnginePINVOKE.Entity_GetAllOfTransform(swigCPtr), true);
    return ret;
  }

	private Behaviour AddScript(string scriptPath)
	{
		ScriptedBehaviour script = AddScriptedBehaviour();
		script.SetScriptClass(scriptPath);

		return script.GetInstance();
	}

	private Behaviour GetScript(string scriptPath)
	{
        ScriptedBehaviourVector scriptedBehaviours = GetAllOfScriptedBehaviour();

        for (int i = 0; i < scriptedBehaviours.Count; i++)
        {
			ScriptedBehaviour script = scriptedBehaviours[i];
			if (script.GetScriptPath() == scriptPath)
				return script.GetInstance();
        }

		return null;
	}

	public string name => GetName();

	public override string ToString() => name;

	public Behaviour AddComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return AddTransform();

		if (type == typeof(CameraComponent))
			return AddCameraComponent();

		return AddScript(type.Name);
	}

	public T AddComponent<T>() where T : Behaviour => AddComponent(typeof(T)) as T;

	public Behaviour GetComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return GetTransform();

		if (type == typeof(CameraComponent))
			return GetCameraComponent();

		return GetScript(type.Name);
	}

	public T GetComponent<T>() where T : Behaviour => GetComponent(typeof(T)) as T;
	
}

}
