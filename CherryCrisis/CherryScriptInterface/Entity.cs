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

  public Entity(global::System.IntPtr cPtr, bool cMemoryOwn) : base(CherryEnginePINVOKE.Entity_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Entity obj) {
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

  public Cell m_cell {
    set {
      CherryEnginePINVOKE.Entity_m_cell_set(swigCPtr, Cell.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_m_cell_get(swigCPtr);
      Cell ret = (cPtr == global::System.IntPtr.Zero) ? null : new Cell(cPtr, false);
      return ret;
    } 
  }

  public Behaviour GetBehaviour(string componentTypeName) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetBehaviour(swigCPtr, componentTypeName);
    Behaviour ret = (cPtr == global::System.IntPtr.Zero) ? null : new Behaviour(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
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

  private ModelRenderer GetModelRenderer() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetModelRenderer(swigCPtr);
    ModelRenderer ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRenderer(cPtr, false);
    return ret;
  }

  private ModelRenderer AddModelRenderer() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddModelRenderer(swigCPtr);
    ModelRenderer ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRenderer(cPtr, false);
    return ret;
  }

  private ModelRendererVector GetAllOfModelRenderer() {
    ModelRendererVector ret = new ModelRendererVector(CherryEnginePINVOKE.Entity_GetAllOfModelRenderer(swigCPtr), true);
    return ret;
  }

  private LightComponent GetLightComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetLightComponent(swigCPtr);
    LightComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponent(cPtr, false);
    return ret;
  }

  private LightComponent AddLightComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddLightComponent(swigCPtr);
    LightComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponent(cPtr, false);
    return ret;
  }

  private LightComponentVector GetAllOfLightComponent() {
    LightComponentVector ret = new LightComponentVector(CherryEnginePINVOKE.Entity_GetAllOfLightComponent(swigCPtr), true);
    return ret;
  }

  private AudioListener GetAudioListener() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetAudioListener(swigCPtr);
    AudioListener ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioListener(cPtr, false);
    return ret;
  }

  private AudioListener AddAudioListener() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddAudioListener(swigCPtr);
    AudioListener ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioListener(cPtr, false);
    return ret;
  }

  private AudioListenerVector GetAllOfAudioListener() {
    AudioListenerVector ret = new AudioListenerVector(CherryEnginePINVOKE.Entity_GetAllOfAudioListener(swigCPtr), true);
    return ret;
  }

  private AudioEmitter GetAudioEmitter() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetAudioEmitter(swigCPtr);
    AudioEmitter ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitter(cPtr, false);
    return ret;
  }

  private AudioEmitter AddAudioEmitter() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddAudioEmitter(swigCPtr);
    AudioEmitter ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitter(cPtr, false);
    return ret;
  }

  private AudioEmitterVector GetAllOfAudioEmitter() {
    AudioEmitterVector ret = new AudioEmitterVector(CherryEnginePINVOKE.Entity_GetAllOfAudioEmitter(swigCPtr), true);
    return ret;
  }

  private PortalComponent GetPortalComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetPortalComponent(swigCPtr);
    PortalComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new PortalComponent(cPtr, false);
    return ret;
  }

  private PortalComponent AddPortalComponent() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddPortalComponent(swigCPtr);
    PortalComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new PortalComponent(cPtr, false);
    return ret;
  }

  private PortalComponentVector GetAllOfPortalComponent() {
    PortalComponentVector ret = new PortalComponentVector(CherryEnginePINVOKE.Entity_GetAllOfPortalComponent(swigCPtr), true);
    return ret;
  }

  private Rigidbody GetRigidbody() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetRigidbody(swigCPtr);
    Rigidbody ret = (cPtr == global::System.IntPtr.Zero) ? null : new Rigidbody(cPtr, false);
    return ret;
  }

  private Rigidbody AddRigidbody() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddRigidbody(swigCPtr);
    Rigidbody ret = (cPtr == global::System.IntPtr.Zero) ? null : new Rigidbody(cPtr, false);
    return ret;
  }

  private RigidbodyVector GetAllOfRigidbody() {
    RigidbodyVector ret = new RigidbodyVector(CherryEnginePINVOKE.Entity_GetAllOfRigidbody(swigCPtr), true);
    return ret;
  }

  private BoxCollider GetBoxCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetBoxCollider(swigCPtr);
    BoxCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new BoxCollider(cPtr, false);
    return ret;
  }

  private BoxCollider AddBoxCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddBoxCollider(swigCPtr);
    BoxCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new BoxCollider(cPtr, false);
    return ret;
  }

  private BoxColliderVector GetAllOfBoxCollider() {
    BoxColliderVector ret = new BoxColliderVector(CherryEnginePINVOKE.Entity_GetAllOfBoxCollider(swigCPtr), true);
    return ret;
  }

  private SphereCollider GetSphereCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetSphereCollider(swigCPtr);
    SphereCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new SphereCollider(cPtr, false);
    return ret;
  }

  private SphereCollider AddSphereCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddSphereCollider(swigCPtr);
    SphereCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new SphereCollider(cPtr, false);
    return ret;
  }

  private SphereColliderVector GetAllOfSphereCollider() {
    SphereColliderVector ret = new SphereColliderVector(CherryEnginePINVOKE.Entity_GetAllOfSphereCollider(swigCPtr), true);
    return ret;
  }

  private CapsuleCollider GetCapsuleCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_GetCapsuleCollider(swigCPtr);
    CapsuleCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new CapsuleCollider(cPtr, false);
    return ret;
  }

  private CapsuleCollider AddCapsuleCollider() {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.Entity_AddCapsuleCollider(swigCPtr);
    CapsuleCollider ret = (cPtr == global::System.IntPtr.Zero) ? null : new CapsuleCollider(cPtr, false);
    return ret;
  }

  private CapsuleColliderVector GetAllOfCapsuleCollider() {
    CapsuleColliderVector ret = new CapsuleColliderVector(CherryEnginePINVOKE.Entity_GetAllOfCapsuleCollider(swigCPtr), true);
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
        Behaviour behaviour = GetBehaviour(scriptPath);
		if (behaviour is null) 
			return null;

        global::System.IntPtr cPtr = Behaviour.getCPtr(behaviour).Handle;
        ScriptedBehaviour scriptedBehaviour = (cPtr == global::System.IntPtr.Zero) ? null : new ScriptedBehaviour(cPtr, false);

        return scriptedBehaviour.GetInstance();
	}

	public string name => GetName();

	public override string ToString() => name;

	public Behaviour AddBehaviour(System.Type type)
	{
		if (type == typeof(Transform))
			return AddTransform();

		if (type == typeof(CameraComponent))
			return AddCameraComponent();

		if (type == typeof(ModelRenderer))
			return AddModelRenderer();

		if (type == typeof(LightComponent))
			return AddLightComponent();
			
		if (type == typeof(AudioListener))
			return AddAudioListener();

		if (type == typeof(AudioEmitter))
			return AddAudioEmitter();

		if (type == typeof(PortalComponent))
			return AddPortalComponent();

		if (type == typeof(Rigidbody))
			return AddRigidbody();

		if (type == typeof(BoxCollider))
			return AddBoxCollider();

		if (type == typeof(SphereCollider))
			return AddSphereCollider();

		if (type == typeof(CapsuleCollider))
			return AddCapsuleCollider();
			
		return AddScript(type.Name);
	}

	public T AddBehaviour<T>() where T : Behaviour => AddBehaviour(typeof(T)) as T;

	public Behaviour GetBehaviour(System.Type type)
	{
		if (type == typeof(Transform))
			return GetTransform();

		if (type == typeof(CameraComponent))
			return GetCameraComponent();

		if (type == typeof(ModelRenderer))
			return GetModelRenderer();
			
		if (type == typeof(LightComponent))
			return GetLightComponent();
						
		if (type == typeof(AudioListener))
			return GetAudioListener();
						
		if (type == typeof(AudioEmitter))
			return GetAudioEmitter();
									
		if (type == typeof(PortalComponent))
			return GetPortalComponent();
			
		if (type == typeof(Rigidbody))
			return GetRigidbody();
			
		if (type == typeof(BoxCollider))
			return GetBoxCollider();
			
		if (type == typeof(SphereCollider))
			return GetSphereCollider();
			
		if (type == typeof(CapsuleCollider))
			return GetCapsuleCollider();
			
		return GetScript(type.Name);
	}

	public T GetBehaviour<T>() where T : Behaviour => GetBehaviour(typeof(T)) as T;
	
}

}
