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

public class Quaternion : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public Quaternion(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Quaternion obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Quaternion() {
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
          CherryEnginePINVOKE.delete_Quaternion(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public float x {
    set {
      CherryEnginePINVOKE.Quaternion_x_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Quaternion_x_get(swigCPtr);
      return ret;
    } 
  }

  public float y {
    set {
      CherryEnginePINVOKE.Quaternion_y_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Quaternion_y_get(swigCPtr);
      return ret;
    } 
  }

  public float z {
    set {
      CherryEnginePINVOKE.Quaternion_z_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Quaternion_z_get(swigCPtr);
      return ret;
    } 
  }

  public float w {
    set {
      CherryEnginePINVOKE.Quaternion_w_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Quaternion_w_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_float data {
    set {
      CherryEnginePINVOKE.Quaternion_data_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Quaternion_data_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

  public Vector3 xyz {
    set {
      CherryEnginePINVOKE.Quaternion_xyz_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Quaternion_xyz_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Quaternion Identity {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Quaternion_Identity_get();
      Quaternion ret = (cPtr == global::System.IntPtr.Zero) ? null : new Quaternion(cPtr, false);
      return ret;
    } 
  }

  public Quaternion() : this(CherryEnginePINVOKE.new_Quaternion__SWIG_0(), true) {
  }

  public Quaternion(float w, float x, float y, float z) : this(CherryEnginePINVOKE.new_Quaternion__SWIG_1(w, x, y, z), true) {
  }

  public Quaternion(float w, Vector3 xyz) : this(CherryEnginePINVOKE.new_Quaternion__SWIG_2(w, Vector3.getCPtr(xyz)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Quaternion(Vector4 v) : this(CherryEnginePINVOKE.new_Quaternion__SWIG_3(Vector4.getCPtr(v)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Quaternion operator_add(Quaternion rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_add(swigCPtr, Quaternion.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Quaternion operator_substract() {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_substract__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Quaternion operator_substract(Quaternion rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_substract__SWIG_1(swigCPtr, Quaternion.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Quaternion operator_multiply(Quaternion rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_multiply__SWIG_0(swigCPtr, Quaternion.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 operator_multiply(Vector3 rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Quaternion_operator_multiply__SWIG_1(swigCPtr, Vector3.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Quaternion operator_multiply(float rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_multiply__SWIG_2(swigCPtr, rhs), true);
    return ret;
  }

  public Quaternion operator_divide(Quaternion rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_divide__SWIG_0(swigCPtr, Quaternion.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Quaternion operator_divide(float rhs) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_operator_divide__SWIG_1(swigCPtr, rhs), true);
    return ret;
  }

  public float Dot(Quaternion rhs) {
    float ret = CherryEnginePINVOKE.Quaternion_Dot__SWIG_0(swigCPtr, Quaternion.getCPtr(rhs));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float SquareNorm() {
    float ret = CherryEnginePINVOKE.Quaternion_SquareNorm(swigCPtr);
    return ret;
  }

  public float Norm() {
    float ret = CherryEnginePINVOKE.Quaternion_Norm(swigCPtr);
    return ret;
  }

  public Quaternion Normalize() {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_Normalize__SWIG_0(swigCPtr), false);
    return ret;
  }

  public Quaternion Normalized() {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_Normalized__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Quaternion Conjugate() {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_Conjugate__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Quaternion Invert() {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_Invert__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Vector3 ToEuler() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Quaternion_ToEuler__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Matrix4 ToMatrix() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Quaternion_ToMatrix__SWIG_0(swigCPtr), true);
    return ret;
  }

  public void ToAxisAngle(Vector3 outAxis, SWIGTYPE_p_float outAngle) {
    CherryEnginePINVOKE.Quaternion_ToAxisAngle__SWIG_0(swigCPtr, Vector3.getCPtr(outAxis), SWIGTYPE_p_float.getCPtr(outAngle));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static Quaternion FromEuler(float roll, float pitch, float yaw) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_FromEuler__SWIG_0(roll, pitch, yaw), true);
    return ret;
  }

  public static Quaternion FromEuler(Vector3 eulerAngles) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_FromEuler__SWIG_1(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion FromAxisAngle(Vector3 inAxis, float inAngle) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_FromAxisAngle(Vector3.getCPtr(inAxis), inAngle), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion FromMatrix(Matrix4 inMatrix) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_FromMatrix(Matrix4.getCPtr(inMatrix)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_FromToRotation(Vector3.getCPtr(fromDirection), Vector3.getCPtr(toDirection)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 RotateVector3(Vector3 inVector) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Quaternion_RotateVector3__SWIG_0(swigCPtr, Vector3.getCPtr(inVector)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion Lerp(Quaternion lhs, Quaternion rhs, float lambda) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_Lerp(Quaternion.getCPtr(lhs), Quaternion.getCPtr(rhs), lambda), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion NLerp(Quaternion lhs, Quaternion rhs, float lambda) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_NLerp(Quaternion.getCPtr(lhs), Quaternion.getCPtr(rhs), lambda), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Quaternion SLerp(Quaternion lhs, Quaternion rhs, float lambda) {
    Quaternion ret = new Quaternion(CherryEnginePINVOKE.Quaternion_SLerp(Quaternion.getCPtr(lhs), Quaternion.getCPtr(rhs), lambda), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

		public override string ToString() => $"Quaternion({x}, {y}, {z}, {w})";

		public static Quaternion operator +(Quaternion lhs, Quaternion rhs) => lhs.operator_add(rhs);
		public static Quaternion operator -(Quaternion lhs, Quaternion rhs) => lhs.operator_substract(rhs);
		public static Quaternion operator *(Quaternion lhs, Quaternion rhs) => lhs.operator_multiply(rhs);
		public static Vector3 operator *(Quaternion lhs, Vector3 rhs) => lhs.operator_multiply(rhs);
		public static Quaternion operator /(Quaternion lhs, Quaternion rhs) => lhs.operator_divide(rhs);
	
}

}
