using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Matrix4 matrix = new Matrix4();

		Transform transform;
		Vector3 maxPosition = new Vector3();
		float speedSensivity = 80.0f;
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) 
			: base(cPtr, cMemoryOwn)
		{

		}

		public void Awake()
		{
			transform = GetBehaviour<Transform>();

		}


		public void Update()
		{
			Debug.GetInstance().Info(transform);
			if (transform == null)
				return;
			
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float sensitityY = Time.GetDeltaTime() * deltaMouse.y;  

			float dt = Time.GetDeltaTime();

			double angleX = transform.eulerAngles.x + sensitityY * speedSensivity * dt;
			angleX = Math.Min(Math.Max(angleX, -Math.PI * 0.4f), Math.PI * 0.4f);
			
			transform.eulerAngles = new Vector3((float)angleX, transform.eulerAngles.y, transform.eulerAngles.z);
		}
	}
}
