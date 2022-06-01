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

public class AudioEmitterVector : global::System.IDisposable, global::System.Collections.IEnumerable, global::System.Collections.Generic.IList<AudioEmitter>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public AudioEmitterVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(AudioEmitterVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~AudioEmitterVector() {
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
          CherryEnginePINVOKE.delete_AudioEmitterVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public AudioEmitterVector(global::System.Collections.IEnumerable c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (AudioEmitter element in c) {
      this.Add(element);
    }
  }

  public AudioEmitterVector(global::System.Collections.Generic.IEnumerable<AudioEmitter> c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (AudioEmitter element in c) {
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

  public AudioEmitter this[int index]  {
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

  public void CopyTo(AudioEmitter[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(AudioEmitter[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, AudioEmitter[] array, int arrayIndex, int count)
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

  public AudioEmitter[] ToArray() {
    AudioEmitter[] array = new AudioEmitter[this.Count];
    this.CopyTo(array);
    return array;
  }

  global::System.Collections.Generic.IEnumerator<AudioEmitter> global::System.Collections.Generic.IEnumerable<AudioEmitter>.GetEnumerator() {
    return new AudioEmitterVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new AudioEmitterVectorEnumerator(this);
  }

  public AudioEmitterVectorEnumerator GetEnumerator() {
    return new AudioEmitterVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class AudioEmitterVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<AudioEmitter>
  {
    private AudioEmitterVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public AudioEmitterVectorEnumerator(AudioEmitterVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public AudioEmitter Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (AudioEmitter)currentObject;
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
    CherryEnginePINVOKE.AudioEmitterVector_Clear(swigCPtr);
  }

  public void Add(AudioEmitter x) {
    CherryEnginePINVOKE.AudioEmitterVector_Add(swigCPtr, AudioEmitter.getCPtr(x));
  }

  private uint size() {
    uint ret = CherryEnginePINVOKE.AudioEmitterVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = CherryEnginePINVOKE.AudioEmitterVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    CherryEnginePINVOKE.AudioEmitterVector_reserve(swigCPtr, n);
  }

  public AudioEmitterVector() : this(CherryEnginePINVOKE.new_AudioEmitterVector__SWIG_0(), true) {
  }

  public AudioEmitterVector(AudioEmitterVector other) : this(CherryEnginePINVOKE.new_AudioEmitterVector__SWIG_1(AudioEmitterVector.getCPtr(other)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public AudioEmitterVector(int capacity) : this(CherryEnginePINVOKE.new_AudioEmitterVector__SWIG_2(capacity), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private AudioEmitter getitemcopy(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.AudioEmitterVector_getitemcopy(swigCPtr, index);
    AudioEmitter ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitter(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private AudioEmitter getitem(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.AudioEmitterVector_getitem(swigCPtr, index);
    AudioEmitter ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitter(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, AudioEmitter val) {
    CherryEnginePINVOKE.AudioEmitterVector_setitem(swigCPtr, index, AudioEmitter.getCPtr(val));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(AudioEmitterVector values) {
    CherryEnginePINVOKE.AudioEmitterVector_AddRange(swigCPtr, AudioEmitterVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public AudioEmitterVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.AudioEmitterVector_GetRange(swigCPtr, index, count);
    AudioEmitterVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitterVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, AudioEmitter x) {
    CherryEnginePINVOKE.AudioEmitterVector_Insert(swigCPtr, index, AudioEmitter.getCPtr(x));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, AudioEmitterVector values) {
    CherryEnginePINVOKE.AudioEmitterVector_InsertRange(swigCPtr, index, AudioEmitterVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    CherryEnginePINVOKE.AudioEmitterVector_RemoveAt(swigCPtr, index);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    CherryEnginePINVOKE.AudioEmitterVector_RemoveRange(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static AudioEmitterVector Repeat(AudioEmitter value, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.AudioEmitterVector_Repeat(AudioEmitter.getCPtr(value), count);
    AudioEmitterVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new AudioEmitterVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    CherryEnginePINVOKE.AudioEmitterVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    CherryEnginePINVOKE.AudioEmitterVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, AudioEmitterVector values) {
    CherryEnginePINVOKE.AudioEmitterVector_SetRange(swigCPtr, index, AudioEmitterVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Contains(AudioEmitter value) {
    bool ret = CherryEnginePINVOKE.AudioEmitterVector_Contains(swigCPtr, AudioEmitter.getCPtr(value));
    return ret;
  }

  public int IndexOf(AudioEmitter value) {
    int ret = CherryEnginePINVOKE.AudioEmitterVector_IndexOf(swigCPtr, AudioEmitter.getCPtr(value));
    return ret;
  }

  public int LastIndexOf(AudioEmitter value) {
    int ret = CherryEnginePINVOKE.AudioEmitterVector_LastIndexOf(swigCPtr, AudioEmitter.getCPtr(value));
    return ret;
  }

  public bool Remove(AudioEmitter value) {
    bool ret = CherryEnginePINVOKE.AudioEmitterVector_Remove(swigCPtr, AudioEmitter.getCPtr(value));
    return ret;
  }

}

}
