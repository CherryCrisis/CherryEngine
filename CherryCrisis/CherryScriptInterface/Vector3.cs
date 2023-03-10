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

public class Vector3 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public Vector3(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Vector3 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Vector3() {
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
          CherryEnginePINVOKE.delete_Vector3(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

	public static Vector3 operator +(Vector3 lhs, Vector3 rhs) => lhs.operator_add(rhs);
	public static Vector3 operator +(Vector3 lhs, float rhs) => lhs.operator_add(rhs);

	public static Vector3 operator -(Vector3 lhs, Vector3 rhs) => lhs.operator_substract(rhs);
	public static Vector3 operator -(Vector3 lhs, float rhs) => lhs.operator_substract(rhs);

	public static Vector3 operator *(Vector3 lhs, float rhs) => lhs.operator_multiply(rhs);
	public static Vector3 operator *(float lhs, Vector3 rhs) => rhs.operator_multiply(lhs);

  public float x {
    set {
      CherryEnginePINVOKE.Vector3_x_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_x_get(swigCPtr);
      return ret;
    } 
  }

  public float y {
    set {
      CherryEnginePINVOKE.Vector3_y_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_y_get(swigCPtr);
      return ret;
    } 
  }

  public float z {
    set {
      CherryEnginePINVOKE.Vector3_z_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_z_get(swigCPtr);
      return ret;
    } 
  }

  public float r {
    set {
      CherryEnginePINVOKE.Vector3_r_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_r_get(swigCPtr);
      return ret;
    } 
  }

  public float g {
    set {
      CherryEnginePINVOKE.Vector3_g_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_g_get(swigCPtr);
      return ret;
    } 
  }

  public float b {
    set {
      CherryEnginePINVOKE.Vector3_b_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_b_get(swigCPtr);
      return ret;
    } 
  }

  public float pitch {
    set {
      CherryEnginePINVOKE.Vector3_pitch_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_pitch_get(swigCPtr);
      return ret;
    } 
  }

  public float yaw {
    set {
      CherryEnginePINVOKE.Vector3_yaw_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_yaw_get(swigCPtr);
      return ret;
    } 
  }

  public float roll {
    set {
      CherryEnginePINVOKE.Vector3_roll_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector3_roll_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_float data {
    set {
      CherryEnginePINVOKE.Vector3_data_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_data_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

  public Vector2 xy {
    set {
      CherryEnginePINVOKE.Vector3_xy_set(swigCPtr, Vector2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_xy_get(swigCPtr);
      Vector2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector2(cPtr, false);
      return ret;
    } 
  }

  public Vector3() : this(CherryEnginePINVOKE.new_Vector3__SWIG_0(), true) {
  }

  public Vector3(float value) : this(CherryEnginePINVOKE.new_Vector3__SWIG_1(value), true) {
  }

  public Vector3(float x, float y, float z) : this(CherryEnginePINVOKE.new_Vector3__SWIG_2(x, y, z), true) {
  }

  public Vector3(Vector2 xy, float z) : this(CherryEnginePINVOKE.new_Vector3__SWIG_3(Vector2.getCPtr(xy), z), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Vector3 operator_add(Vector3 rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_add__SWIG_0(swigCPtr, Vector3.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 operator_add(float rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_add__SWIG_1(swigCPtr, rhs), true);
    return ret;
  }

  public Vector3 operator_substract() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_substract__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Vector3 operator_substract(Vector3 rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_substract__SWIG_1(swigCPtr, Vector3.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 operator_substract(float rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_substract__SWIG_2(swigCPtr, rhs), true);
    return ret;
  }

  public float operator_multiply(Vector3 rhs) {
    float ret = CherryEnginePINVOKE.Vector3_operator_multiply__SWIG_0(swigCPtr, Vector3.getCPtr(rhs));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 operator_multiply(float rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_operator_multiply__SWIG_1(swigCPtr, rhs), true);
    return ret;
  }

  public float Dot(Vector3 rhs) {
    float ret = CherryEnginePINVOKE.Vector3_Dot__SWIG_0(swigCPtr, Vector3.getCPtr(rhs));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 Multiply(Vector3 rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_Multiply__SWIG_0(swigCPtr, Vector3.getCPtr(rhs)), false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 Cross(Vector3 rhs) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_Cross__SWIG_0(swigCPtr, Vector3.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float SquareLength() {
    float ret = CherryEnginePINVOKE.Vector3_SquareLength(swigCPtr);
    return ret;
  }

  public float Length() {
    float ret = CherryEnginePINVOKE.Vector3_Length(swigCPtr);
    return ret;
  }

  public void ClampLength(float minLength, float maxLength) {
    CherryEnginePINVOKE.Vector3_ClampLength__SWIG_0(swigCPtr, minLength, maxLength);
  }

  public static Vector3 Lerp(Vector3 lhs, Vector3 rhs, float lambda) {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_Lerp(Vector3.getCPtr(lhs), Vector3.getCPtr(rhs), lambda), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector3 Normalize() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_Normalize__SWIG_0(swigCPtr), false);
    return ret;
  }

  public Vector3 Normalized() {
    Vector3 ret = new Vector3(CherryEnginePINVOKE.Vector3_Normalized__SWIG_0(swigCPtr), true);
    return ret;
  }

  public static Vector3 XAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_XAxis_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 YAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_YAxis_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 ZAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_ZAxis_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 Forward {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_Forward_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 Up {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_Up_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 Right {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_Right_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 Zero {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_Zero_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public static Vector3 One {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector3_One_get();
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

		public override string ToString() => $"Vector3({x}, {y}, {z})";
	
}

}
