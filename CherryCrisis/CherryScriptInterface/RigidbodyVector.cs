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

public class RigidbodyVector : global::System.IDisposable, global::System.Collections.IEnumerable, global::System.Collections.Generic.IList<Rigidbody>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public RigidbodyVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(RigidbodyVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~RigidbodyVector() {
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
          CherryEnginePINVOKE.delete_RigidbodyVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public RigidbodyVector(global::System.Collections.IEnumerable c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (Rigidbody element in c) {
      this.Add(element);
    }
  }

  public RigidbodyVector(global::System.Collections.Generic.IEnumerable<Rigidbody> c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (Rigidbody element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public Rigidbody this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new global::System.ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(Rigidbody[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(Rigidbody[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, Rigidbody[] array, int arrayIndex, int count)
  {
    if (array == null)
      throw new global::System.ArgumentNullException("array");
    if (index < 0)
      throw new global::System.ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new global::System.ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new global::System.ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new global::System.ArgumentException("Multi dimensional array.", "array");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new global::System.ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  public Rigidbody[] ToArray() {
    Rigidbody[] array = new Rigidbody[this.Count];
    this.CopyTo(array);
    return array;
  }

  global::System.Collections.Generic.IEnumerator<Rigidbody> global::System.Collections.Generic.IEnumerable<Rigidbody>.GetEnumerator() {
    return new RigidbodyVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new RigidbodyVectorEnumerator(this);
  }

  public RigidbodyVectorEnumerator GetEnumerator() {
    return new RigidbodyVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class RigidbodyVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<Rigidbody>
  {
    private RigidbodyVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public RigidbodyVectorEnumerator(RigidbodyVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public Rigidbody Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (Rigidbody)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object global::System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new global::System.InvalidOperationException("Collection modified.");
      }
    }

    public void Dispose() {
        currentIndex = -1;
        currentObject = null;
    }
  }

  public void Clear() {
    CherryEnginePINVOKE.RigidbodyVector_Clear(swigCPtr);
  }

  public void Add(Rigidbody x) {
    CherryEnginePINVOKE.RigidbodyVector_Add(swigCPtr, Rigidbody.getCPtr(x));
  }

  private uint size() {
    uint ret = CherryEnginePINVOKE.RigidbodyVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = CherryEnginePINVOKE.RigidbodyVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    CherryEnginePINVOKE.RigidbodyVector_reserve(swigCPtr, n);
  }

  public RigidbodyVector() : this(CherryEnginePINVOKE.new_RigidbodyVector__SWIG_0(), true) {
  }

  public RigidbodyVector(RigidbodyVector other) : this(CherryEnginePINVOKE.new_RigidbodyVector__SWIG_1(RigidbodyVector.getCPtr(other)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public RigidbodyVector(int capacity) : this(CherryEnginePINVOKE.new_RigidbodyVector__SWIG_2(capacity), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private Rigidbody getitemcopy(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.RigidbodyVector_getitemcopy(swigCPtr, index);
    Rigidbody ret = (cPtr == global::System.IntPtr.Zero) ? null : new Rigidbody(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private Rigidbody getitem(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.RigidbodyVector_getitem(swigCPtr, index);
    Rigidbody ret = (cPtr == global::System.IntPtr.Zero) ? null : new Rigidbody(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, Rigidbody val) {
    CherryEnginePINVOKE.RigidbodyVector_setitem(swigCPtr, index, Rigidbody.getCPtr(val));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(RigidbodyVector values) {
    CherryEnginePINVOKE.RigidbodyVector_AddRange(swigCPtr, RigidbodyVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public RigidbodyVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.RigidbodyVector_GetRange(swigCPtr, index, count);
    RigidbodyVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new RigidbodyVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, Rigidbody x) {
    CherryEnginePINVOKE.RigidbodyVector_Insert(swigCPtr, index, Rigidbody.getCPtr(x));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, RigidbodyVector values) {
    CherryEnginePINVOKE.RigidbodyVector_InsertRange(swigCPtr, index, RigidbodyVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    CherryEnginePINVOKE.RigidbodyVector_RemoveAt(swigCPtr, index);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    CherryEnginePINVOKE.RigidbodyVector_RemoveRange(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static RigidbodyVector Repeat(Rigidbody value, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.RigidbodyVector_Repeat(Rigidbody.getCPtr(value), count);
    RigidbodyVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new RigidbodyVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    CherryEnginePINVOKE.RigidbodyVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    CherryEnginePINVOKE.RigidbodyVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, RigidbodyVector values) {
    CherryEnginePINVOKE.RigidbodyVector_SetRange(swigCPtr, index, RigidbodyVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Contains(Rigidbody value) {
    bool ret = CherryEnginePINVOKE.RigidbodyVector_Contains(swigCPtr, Rigidbody.getCPtr(value));
    return ret;
  }

  public int IndexOf(Rigidbody value) {
    int ret = CherryEnginePINVOKE.RigidbodyVector_IndexOf(swigCPtr, Rigidbody.getCPtr(value));
    return ret;
  }

  public int LastIndexOf(Rigidbody value) {
    int ret = CherryEnginePINVOKE.RigidbodyVector_LastIndexOf(swigCPtr, Rigidbody.getCPtr(value));
    return ret;
  }

  public bool Remove(Rigidbody value) {
    bool ret = CherryEnginePINVOKE.RigidbodyVector_Remove(swigCPtr, Rigidbody.getCPtr(value));
    return ret;
  }

}

}