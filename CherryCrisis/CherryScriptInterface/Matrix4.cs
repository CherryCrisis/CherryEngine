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

public class Matrix4 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public Matrix4(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(Matrix4 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Matrix4() {
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
          CherryEnginePINVOKE.delete_Matrix4(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public SWIGTYPE_p_float data {
    set {
      CherryEnginePINVOKE.Matrix4_data_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_data_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

  public Vector4 row {
    set {
      CherryEnginePINVOKE.Matrix4_row_set(swigCPtr, Vector4.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_row_get(swigCPtr);
      Vector4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector4(cPtr, false);
      return ret;
    } 
  }

  public Vector3 right {
    set {
      CherryEnginePINVOKE.Matrix4_right_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_right_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float m4 {
    set {
      CherryEnginePINVOKE.Matrix4_m4_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Matrix4_m4_get(swigCPtr);
      return ret;
    } 
  }

  public Vector3 up {
    set {
      CherryEnginePINVOKE.Matrix4_up_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_up_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float m8 {
    set {
      CherryEnginePINVOKE.Matrix4_m8_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Matrix4_m8_get(swigCPtr);
      return ret;
    } 
  }

  public Vector3 back {
    set {
      CherryEnginePINVOKE.Matrix4_back_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_back_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float m12 {
    set {
      CherryEnginePINVOKE.Matrix4_m12_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Matrix4_m12_get(swigCPtr);
      return ret;
    } 
  }

  public Vector3 position {
    set {
      CherryEnginePINVOKE.Matrix4_position_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_position_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float uniformScale {
    set {
      CherryEnginePINVOKE.Matrix4_uniformScale_set(swigCPtr, value);
    } 
    get {
      float ret = CherryEnginePINVOKE.Matrix4_uniformScale_get(swigCPtr);
      return ret;
    } 
  }

  public static Matrix4 Identity {
    get {
      global::System.IntPtr cPtr = CherryEnginePINVOKE.Matrix4_Identity_get();
      Matrix4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Matrix4(cPtr, false);
      return ret;
    } 
  }

  public Vector4 operator_multiply(Vector4 rhs) {
    Vector4 ret = new Vector4(CherryEnginePINVOKE.Matrix4_operator_multiply__SWIG_0(swigCPtr, Vector4.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Matrix4 operator_multiply(Matrix4 rhs) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_operator_multiply__SWIG_1(swigCPtr, Matrix4.getCPtr(rhs)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Matrix4 operator_multiply(float rhs) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_operator_multiply__SWIG_2(swigCPtr, rhs), true);
    return ret;
  }

  public Matrix4 Transpose() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Transpose__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Matrix4 Inverse() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Inverse__SWIG_0(swigCPtr), true);
    return ret;
  }

  public static Matrix4 Translate(Vector3 inPosition) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Translate(Vector3.getCPtr(inPosition)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 Scale(float inScale) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Scale__SWIG_0(inScale), true);
    return ret;
  }

  public static Matrix4 Scale(Vector3 inScale) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Scale__SWIG_1(Vector3.getCPtr(inScale)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void NormalizeScale() {
    CherryEnginePINVOKE.Matrix4_NormalizeScale(swigCPtr);
  }

  public Matrix4 NormalizedScale() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_NormalizedScale(swigCPtr), true);
    return ret;
  }

  public static Matrix4 RotateXYZ(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateXYZ(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateZYX(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateZYX(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateZXY(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateZXY(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateYXZ(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateYXZ(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateXZY(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateXZY(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateYZX(Vector3 eulerAngles) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateYZX(Vector3.getCPtr(eulerAngles)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 RotateX(float rad) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateX__SWIG_0(rad), true);
    return ret;
  }

  public static Matrix4 RotateY(float rad) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateY__SWIG_0(rad), true);
    return ret;
  }

  public static Matrix4 RotateZ(float rad) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateZ__SWIG_0(rad), true);
    return ret;
  }

  public static Matrix4 RotateX(float cos, float sin) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateX__SWIG_1(cos, sin), true);
    return ret;
  }

  public static Matrix4 RotateY(float cos, float sin) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateY__SWIG_1(cos, sin), true);
    return ret;
  }

  public static Matrix4 RotateZ(float cos, float sin) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_RotateZ__SWIG_1(cos, sin), true);
    return ret;
  }

  public static void Decompose(Matrix4 matrix, Vector3 outPos, Vector3 outRot, Vector3 outScale) {
    CherryEnginePINVOKE.Matrix4_Decompose(Matrix4.getCPtr(matrix), Vector3.getCPtr(outPos), Vector3.getCPtr(outRot), Vector3.getCPtr(outScale));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public Matrix4 GetRotationMatrix() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_GetRotationMatrix__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Matrix4 GetTranslationMatrix() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_GetTranslationMatrix__SWIG_0(swigCPtr), true);
    return ret;
  }

  public Matrix4 GetScaleMatrix() {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_GetScaleMatrix__SWIG_0(swigCPtr), true);
    return ret;
  }

  public static Matrix4 Frustum(float Left, float Right, float Bottom, float Top, float Near, float Far) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Frustum(Left, Right, Bottom, Top, Near, Far), true);
    return ret;
  }

  public static Matrix4 Perspective(float FovY, float Aspect, float Near, float Far) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Perspective(FovY, Aspect, Near, Far), true);
    return ret;
  }

  public static Matrix4 Orthographic(float Left, float Right, float Bottom, float Top, float Near, float Far) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_Orthographic(Left, Right, Bottom, Top, Near, Far), true);
    return ret;
  }

  public static Matrix4 ObliqueProjection(Vector4 clipPlane, Matrix4 projection) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_ObliqueProjection(Vector4.getCPtr(clipPlane), Matrix4.getCPtr(projection)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 LookAt(Vector3 Eye, Vector3 At, Vector3 Up) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_LookAt__SWIG_0(Vector3.getCPtr(Eye), Vector3.getCPtr(At), Vector3.getCPtr(Up)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 LookAt(Vector3 Eye, Vector3 At) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_LookAt__SWIG_1(Vector3.getCPtr(Eye), Vector3.getCPtr(At)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Matrix4 LookAt(Vector3 Eye) {
    Matrix4 ret = new Matrix4(CherryEnginePINVOKE.Matrix4_LookAt__SWIG_2(Vector3.getCPtr(Eye)), true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

		public static Matrix4 operator *(Matrix4 lhs, Matrix4 rhs) => lhs.operator_multiply(rhs);
	
  public Matrix4() : this(CherryEnginePINVOKE.new_Matrix4(), true) {
  }

}

}
