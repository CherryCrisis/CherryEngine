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

public class Vector4 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Vector4(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Vector4 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Vector4() {
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
          CherryEnginePINVOKE.delete_Vector4(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

	public static Vector4 operator +(Vector4 lhs, Vector4 rhs) => lhs.operator_add(rhs);
	public static Vector4 operator +(Vector4 lhs, float rhs) => lhs.operator_add(rhs);

	public static Vector4 operator -(Vector4 lhs, Vector4 rhs) => lhs.operator_substract(rhs);
	public static Vector4 operator -(Vector4 lhs, float rhs) => lhs.operator_substract(rhs);


  public float x {
    set {
      CherryEnginePINVOKE.Vector4_x_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_x_get(swigCPtr);
      return ret;
    } 
  }

  public float y {
    set {
      CherryEnginePINVOKE.Vector4_y_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_y_get(swigCPtr);
      return ret;
    } 
  }

  public float z {
    set {
      CherryEnginePINVOKE.Vector4_z_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_z_get(swigCPtr);
      return ret;
    } 
  }

  public float w {
    set {
      CherryEnginePINVOKE.Vector4_w_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_w_get(swigCPtr);
      return ret;
    } 
  }

  public float r {
    set {
      CherryEnginePINVOKE.Vector4_r_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_r_get(swigCPtr);
      return ret;
    } 
  }

  public float g {
    set {
      CherryEnginePINVOKE.Vector4_g_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_g_get(swigCPtr);
      return ret;
    } 
  }

  public float b {
    set {
      CherryEnginePINVOKE.Vector4_b_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_b_get(swigCPtr);
      return ret;
    } 
  }

  public float a {
    set {
      CherryEnginePINVOKE.Vector4_a_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Vector4_a_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_float data {
    set {
      CherryEnginePINVOKE.Vector4_data_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_data_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

  public Vector3 xyz {
    set {
      CherryEnginePINVOKE.Vector4_xyz_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_xyz_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public Vector3 rgb {
    set {
      CherryEnginePINVOKE.Vector4_rgb_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_rgb_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public Vector4() : this(CherryEnginePINVOKE.new_Vector4__SWIG_0(), true) {
  }

  public Vector4(float value) : this(CherryEnginePINVOKE.new_Vector4__SWIG_1(value), true) {
  }

  public Vector4(float x, float y, float z, float w) : this(CherryEnginePINVOKE.new_Vector4__SWIG_2(x, y, z, w), true) {
  }

  public Vector4(Vector3 xyz, float w) : this(CherryEnginePINVOKE.new_Vector4__SWIG_3(Vector3.getCPtr(xyz), w), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Vector4 operator_add(Vector4 rhs) {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_operator_add__SWIG_0(swigCPtr, Vector4.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector4 operator_add(float rhs) {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_operator_add__SWIG_1(swigCPtr, rhs), true);
    return ret;
  }

  public Vector4 operator_substract() {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_operator_substract__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Vector4 operator_substract(Vector4 rhs) {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_operator_substract__SWIG_1(swigCPtr, Vector4.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector4 operator_substract(float rhs) {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_operator_substract__SWIG_2(swigCPtr, rhs), true);
    return ret;
  }

  public float Dot(Vector4 rhs) {
    float ret = CherryEnginePINVOKE.Vector4_Dot__SWIG_0(swigCPtr, Vector4.getCPtr(rhs));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float SquareLength() {
    float ret = CherryEnginePINVOKE.Vector4_SquareLength(swigCPtr);
    return ret;
  }

  public float Length() {
    float ret = CherryEnginePINVOKE.Vector4_Length(swigCPtr);
    return ret;
  }

  public Vector4 Normalize() {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_Normalize__SWIG_0(swigCPtr), false);
    return ret;
  }

  public Vector4 Normalized() {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Vector4_Normalized__SWIG_0(swigCPtr), true);
    return ret;
  }

  public static Vector4 XAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_XAxis_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public static Vector4 YAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_YAxis_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public static Vector4 ZAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_ZAxis_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public static Vector4 WAxis {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_WAxis_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public static Vector4 Zero {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_Zero_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public static Vector4 One {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Vector4_One_get();
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

		public override string ToString() => $"Vector4({x}, {y}, {z}, {w})";
	
}

}
