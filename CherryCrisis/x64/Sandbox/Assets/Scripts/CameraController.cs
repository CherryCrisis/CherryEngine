using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn)
		: base(cPtr, cMemoryOwn) { }

		Transform transform;
		Transform cubeOffset;

		PickableCube pickedCube;
		float throwingStrength = 5f;

		float speedSensivity = 80.0f;
		float interactRange = 25f;

		public void Awake() => transform = GetBehaviour<Transform>();

		public void Update()
		{
			if (transform == null) return;

			UpdateCameraRotation();

			if (InputManager.GetKeyDown("Interact")) TryInteract();

			if (InputManager.GetKeyDown(Keycode.LEFT_CLICK)) TryAction();

			if (InputManager.GetKeyDown(Keycode.ESCAPE)) 
			{ 
				InputManager.SetCursorDisplayed();
				InputManager.SetUIFocus(true);
			}
		}

		void TryInteract()
		{
			RaycastHit hit = PhysicManager.Raycast(GetHost().m_cell, transform.GetGlobalPosition(), transform.Forward().Normalized(), interactRange);
			if (hit == null || hit.actor == null || hit.actor.m_owner == null)
				return;
			PortalSwitcher switcher = hit.actor.m_owner.GetBehaviour<PortalSwitcher>();

			switcher?.Switch();	

			Wardrobe wardrobe = hit.actor.m_owner.GetBehaviour<Wardrobe>();
			wardrobe?.SetInMovement();
			
			CubeSpawner spawner = hit.actor.m_owner.GetBehaviour<CubeSpawner>();
			spawner?.SpawnCube(); 
			
			PickableCube cube = hit.actor.m_owner.GetBehaviour<PickableCube>();
			if (cube != null) 
				PickupCube(cube);
		}

		void TryAction() 
		{
			if (pickedCube != null)
				ThrowCube();
		}

		void UpdateCameraRotation() 
		{
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float dt = Time.GetDeltaTime();

			float sensitityY = deltaMouse.y;
			double angleX = transform.eulerAngles.x + sensitityY * speedSensivity * 0.002f;
			angleX = Math.Min(Math.Max(angleX, -Math.PI * 0.4f), Math.PI * 0.4f);

			transform.eulerAngles = new Vector3((float)angleX, transform.eulerAngles.y, transform.eulerAngles.z);
		}

		void PickupCube(PickableCube cube) 
		{
			cube.Pick(cubeOffset);
			pickedCube = cube;
		}

		void ThrowCube()
		{
			pickedCube.Throw(transform.Forward().Normalized(), throwingStrength);
			pickedCube = null;
		}
	}
}
