using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Matrix4 matrix = new Matrix4();

		Transform transform;
		Vector3 maxPosition = new Vector3();
		int step = 0;
		float speed = 2.0f;
		float speedSensivity = 80.0f;
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) 
			: base(cPtr, cMemoryOwn)
		{

		}

		public void Start()
		{
			transform = GetComponent<Transform>();
		}


		public void Update()
		{
			Debug.GetInstance().Info(transform);
			if (transform == null)
				return;
			
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float sensitityY = Time.GetDeltaTime() * deltaMouse.y;  

			float dt = Time.GetDeltaTime();

			float sign = 1;

			if (InputManager.GetKey(Keycode.W))
			{
				sign = 1;
				Debug.GetInstance().Log(ELogType.WARNING, "Code Wfgko Coucou Bem mets nous 20/20 (stp)");
			}
			else if (InputManager.GetKey(Keycode.S))
				sign = -1;
			else
				sign = 0;

			float x = sign * transform.Forward().x * speed * dt;
			float y = sign * transform.Forward().y * speed * dt;
			float z = sign * transform.Forward().z * speed * dt;
			transform.SetPosition(transform.position +  new Vector3(x,y,z));

			float sensitityX = Time.GetDeltaTime() * deltaMouse.x;

			double angleX = transform.eulerAngles.x + sensitityY * speedSensivity * dt;
			angleX = Math.Min(Math.Max(angleX, -Math.PI * 0.4f), Math.PI * 0.4f);
			
			transform.eulerAngles = new Vector3((float)angleX, transform.eulerAngles.y, transform.eulerAngles.z);

			double angleY = transform.eulerAngles.y + sensitityX * speedSensivity * dt;
			transform.eulerAngles = new Vector3(transform.eulerAngles.x, (float)angleY, transform.eulerAngles.z);

			transform.eulerAngles = Quaternion.Identity * transform.eulerAngles;

			Debug.GetInstance().Info(GetHost().m_cell);

			RaycastHit hit = PhysicManager.Raycast(GetHost().m_cell, transform.position, transform.Forward(), 5000f);
			PhysicActor a = hit.actor;

			if (a != null)
				Debug.GetInstance().Info(a.m_owner);
		}
	}
}
