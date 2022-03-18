#include "Player.h"
#include "SceneManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Shot.h"
#include "EffectManager.h"
#include "Navigation.h"

Player::Player()
{
	PP::Model::Make(model, "PLAYER");
	//efc = new Effect01();
	//EffectManager::Instance().Register(efc);

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetPosition({ 0.0f, 0.0f, 0.5f });
	SetRadius(0.5f);
	SetHeight(1.4f);
	SetHealth(5);
	SetMaxHealth(GetHealth());
	stamina = 50;
	maxStamina = stamina;
	PP::Model::UpdateTransform(model, wc);
	TransIdleState(); // 初期の状態設定
}

Player::~Player()
{
	//delete model;
}

void Player::Update(float elapsedTime)
{
	UpdateCameraState(elapsedTime);

	PP::Model::UpdateAnimation(model, elapsedTime);

	PP::Model::UpdateSubAnimation(model, elapsedTime);

	switch (state)
	{
	case State::IDLE:
		UpdateIdleState(elapsedTime);
		break;
	case State::WALK:
		UpdateWalkState(elapsedTime);
		break;
	case State::RUN:
		UpdateRunState(elapsedTime);
		break;
	case State::DASH:
		UpdateDashState(elapsedTime);
		break;
	case State::JUMP:
		//UpdateJumpState(elapsedTime);
		break;
	case State::FALL:
		UpdateFallState(elapsedTime);
		break;
	case State::LAND:
		UpdateLandState(elapsedTime);
		break;
	case State::ATTACK:
		AttackMoveUpdate(elapsedTime);
		break;
	case State::DODGE:
		UpdateDodgeState(elapsedTime);
		break;
	case State::DAMAGE:
		UpdateDamageState(elapsedTime);
		break;
	case State::GUARD_IDLE:
		UpdateGuardIdleState(elapsedTime);
		break;
	case State::GUARD_WALK:
		UpdateGuardWalkState(elapsedTime);
		break;
	case State::GUARD_RUN:
		UpdateGuardRunState(elapsedTime);
		break;
	case State::GUARD_DASH:
		UpdateGuardDashState(elapsedTime);
		break;
	case State::GUARD_REACTION:
		UpdateGuardReactionState(elapsedTime);
		break;
	case State::DEATH:
		UpdateDeathState(elapsedTime);
		break;
	}

	// 仮置き // 後ほど各状態関数内で呼び出す?
	if (FallCheck() && state != State::DODGE)
		TransFallState();

	//if (PP::Input::CheckHitKey('Q') == 1) // 落下死確認用処理
	//{
	//	velocity.y = 0;
	//	wc.position.y += 15;
	//}

	UpdateHeal();

	UpdateStamina();

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	CollisionPlayerVsEnemies();
	
	PP::Model::UpdateTransform(model, wc);
}

void Player::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void Player::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void Player::DebugRender()
{
	//if (invincibleTimer <= 0) // 無敵時間がない場合表示
	//	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius * 30, height, { 0, 0, 0, 1 });

	// レイの描画
	/*{
		float my = velocity.y * PP::System::GetElapsedTime();

		DirectX::XMFLOAT3 start = { wc.position.x , wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end = { wc.position.x, wc.position.y + my + gravity, wc.position.z };

		float mx = velocity.x * PP::System::GetElapsedTime();
		float mz = velocity.z * PP::System::GetElapsedTime();

		DirectX::XMFLOAT3 start2 = { wc.position.x, wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end2 = { wc.position.x + mx, wc.position.y + stepOffset, wc.position.z + mz };

		PP::Debug::Primitive3D::DrawLine(start, end, { 1, 1, 0, 1 });

		PP::Debug::Primitive3D::DrawLine(start2, end2, { 1, 1, 0, 1 });
	}*/

	// 前方ベクトル
	{
		DirectX::XMFLOAT3 foward = PP::Model::GetForward(model);
		DirectX::XMFLOAT3 v = { wc.position.x + foward.x * 30, wc.position.y + foward.y * 30, wc.position.z + foward.z * 30 };
		PP::Debug::Primitive3D::DrawLine(wc.position, v, { 1, 1, 0, 1 });
	}

	Navigation::Instance().DebugRender();

	if (attackCollisionFlag) // 攻撃判定がある時に判定範囲の表示
		PP::Debug::Primitive3D::DrawSphere(weaponPosition, nodeRadius, { 1, 0, 0, 1 });

	PP::Imgui::Begin(u8"PlayerDebug", { 0, 0 }, { 128, 256 });
	PP::Imgui::Text("Position\n x:%2f\n y:%2f\n z:%2f", wc.position.x, wc.position.y, wc.position.z);
	PP::Imgui::Text("Velocity\n x:%2f\n y:%2f\n z:%2f", velocity.x, velocity.y, velocity.z);
	PP::Imgui::Text("fallTimer : %f", fallTimer);
	PP::Imgui::Text("healItem : %d", healItem);
	switch (state)
	{
	case State::IDLE:			PP::Imgui::Text(u8"Idle");				break;
	case State::WALK:			PP::Imgui::Text(u8"Walk");				break;
	case State::RUN:			PP::Imgui::Text(u8"Run");				break;
	case State::DASH:			PP::Imgui::Text(u8"DASH");				break;
	case State::JUMP:			PP::Imgui::Text(u8"JUMP");				break;
	case State::FALL:			PP::Imgui::Text(u8"FALL");				break;
	case State::LAND:			PP::Imgui::Text(u8"LAND");				break;
	case State::ATTACK:			PP::Imgui::Text(u8"ATTACK");			break;
	case State::DODGE:			PP::Imgui::Text(u8"DODGE");				break;
	case State::DAMAGE:			PP::Imgui::Text(u8"DAMAGE");			break;
	case State::GUARD_IDLE:		PP::Imgui::Text(u8"GUARD_IDLE");		break;
	case State::GUARD_WALK:		PP::Imgui::Text(u8"GUARD_WALK");		break;
	case State::GUARD_RUN:		PP::Imgui::Text(u8"GUARD_RUN");			break;
	case State::GUARD_DASH:		PP::Imgui::Text(u8"GUARD_DASH");		break;
	case State::GUARD_REACTION:	PP::Imgui::Text(u8"GUARD_REACTION");	break;
	case State::DEATH:			PP::Imgui::Text(u8"DEATH");				break;
	}
	PP::Imgui::End();
}

bool Player::CheckGuard(int damage, float invincibleTime)
{
	if (!guardFlag) return false; // ガードしていない場合はfalse;

	if (invincibleTimer > 0.0f) return false; // 無敵中のためfalse

	invincibleTimer = invincibleTime;

	// ガード時のスタミナ消費と回復し始めるまでの時間
	UseStamina(damage, 7.0f); // スタミナ処理

	OnGuarded(); // ガード通知

	return true;
}

void Player::OnGuarded()
{
	// ガードリアクションステートへ遷移
	TransGuardReactionState();
}

void Player::OnDamaged()
{
	// ダメージステートへ遷移
	TransDamageState();
}

void Player::OnDead()
{
	// 死亡ステートへ遷移
	TransDeathState();
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	const float fallHeight = 5.0f;
	// 仮実装(一定加速度もしくは一定時間以上の落下で死亡にする？)
	if (fallTimer > fallHeight) TransDeath2State();

	fallTimer = 0.0f;

	// 被弾時、死亡時、回避時でなければ着地可能
	if (state != State::DAMAGE && state != State::DEATH && state != State::DODGE)
	{
		TransLandState();
	}
}

// 入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	float ax = PP::Input::GetLeftStickX();
	float ay = PP::Input::GetLeftStickY();

	// カメラの向きとスティック入力で進行方向を算出
	const DirectX::XMFLOAT3& cameraRight = PP::Camera::GetRight();
	const DirectX::XMFLOAT3& cameraFront = PP::Camera::GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf((cameraRightX * cameraRightX) + (cameraRightZ * cameraRightZ));
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf((cameraFrontX * cameraFrontX) + (cameraFrontZ * cameraFrontZ));
	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.y = 0.0f;
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);

	return vec;
}

void Player::UpdateHeal()
{
	if (InputHeal()) // Healing()的な関数にまとめる
	{
		int health = GetHealth();
		int maxHealth = GetMaxHealth();
		health += maxHealth * 0.5f;
		healItem--;
		if (health >= maxHealth)
			health = maxHealth;
		SetHealth(health);
	}
}

void Player::UpdateStamina() // スタミナ自然回復
{
	if (stamina >= maxStamina) // 最大なら返す
	{
		stamina = maxStamina; 
		return;
	}

	if (stamina <= 0) stamina = 0.0f;

	if (state == State::ATTACK || state == State::DODGE) return;

	staminaTimer--;

	if (staminaTimer > 0) return; // 回復し始めるまでの時間がある場合は返す

	// スタミナに回復速度を加算
	if (guardFlag) stamina += staminaHeal / 2;
	else stamina += staminaHeal;
}

void Player::UseStamina(float use, float timer)
{
	stamina -= use;
	staminaTimer = timer;
}

bool Player::InputMove(float elapsedTime)
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(moveVec.x, moveVec.z, speed);

	Turn(moveVec.x, moveVec.z, turnSpeed, elapsedTime);

	if (lockonFlag) // ロックオン時でもダッシュは移動ベクトルの方向を向く
	{
		// プレイヤーの座標からロックオン先の敵
		const DirectX::XMFLOAT2 lockVec = { 
			lockonEnemy->GetPosition().x - wc.position.x,
			lockonEnemy->GetPosition().z - wc.position.z };

		const float turnSpeed2 = turnSpeed * 2.0f;
		const float turnSpeed10 = turnSpeed * 10.0f;
		// ロックオン対象の方を向く
		if (state != State::DASH)
		{
			// しっかり敵の方向へ回転
			Turn(lockVec.x, lockVec.y, turnSpeed10, elapsedTime);
		}
		else 
			Turn(moveVec.x, moveVec.z, turnSpeed2, elapsedTime);
	}

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

bool Player::InputWalk()
{
	float ax = PP::Input::GetLeftStickX();
	float ay = PP::Input::GetLeftStickY();

	float l = sqrtf(ax * ax + ay * ay);
	const float walkRange = 0.8f; // 歩行時の最大入力

	if (l < walkRange)
	{
		return true;
	}
	return false;
}

bool Player::InputDash()
{
	if (stamina <= 0) return false; // スタミナがなければfalse

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButton() & GamePad::BTN_RIGHT_SHOULDER)
	{
		return true;
	}
	return false;
}

bool Player::InputJump()
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	/*if (pad.GetButtonDown() & GamePad::BTN_A)
	{
			Jump(jumpSpeed);
			return true; // 入力をした
	}*/
	return false;
}

bool Player::InputAttack()
{
	if (stamina <= 0) return false; // スタミナがなければfalse

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_X)
	{
		return true;
	}
	return false;
}

bool Player::InputGuard() // 継続
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButton() & GamePad::BTN_LEFT_SHOULDER || PP::Input::CheckHitKey('B'))
	{
		return true;
	}
	return false;
}

bool Player::InputHeal()
{
	if (healItem <= 0) return false;
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_Y) // 仮キー
	{
		return true; // 入力をした
	}

	return false;
}

bool Player::InputDodge() 
{
	if (stamina <= 0) return false; // スタミナがなければfalse

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

bool Player::FallCheck()
{
	// 地面にいないかつ下への加速度がある
	return !IsGround() && (velocity.y) < 0.0f;
}

void Player::InputProjectile()
{
	 // たぶんつかわない
}

void Player::UpdateCameraState(float elapsedTime)
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	bool oldLockonFlag = lockonFlag;
	Enemy* oldLockonEnemy = lockonEnemy;

	lockonFlag = false;
	lockonEnemy = nullptr;

	switch (state)
	{
	case State::IDLE:
	case State::WALK:
	case State::RUN:
	case State::DASH:
	case State::JUMP:
	case State::FALL:
	case State::LAND:
	case State::ATTACK:
	case State::DODGE:
	case State::DAMAGE:
	case State::GUARD_IDLE:
	case State::GUARD_WALK:
	case State::GUARD_RUN:
	case State::GUARD_DASH:
	case State::GUARD_REACTION:
	case State::DEATH:
		// ロックオンフラグ切り替え
		if (pad.GetButtonDown() & GamePad::BTN_RIGHT_THUMB || PP::Input::CheckHitKey('R') == 1)
		{
			lockonButtonFlag ^= true;
		}
		// ロックオンモード
		if (lockonButtonFlag)
		{
			DirectX::XMVECTOR p, t, v;
			if (!oldLockonFlag) // 前回ロックオンしてないなら
			{
				// (プレイヤーから一定範囲内の)一番近い敵を検索
				float length1, length2;
				EnemyManager& enemyManager = EnemyManager::Instance();
				for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
				{
					Enemy* enemy = enemyManager.GetEnemy(i);

					if (lockonFlag) // 今回ロックオンしている
					{
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&lockonEnemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p); // ターゲット - プレイヤー
						DirectX::XMStoreFloat(&length2, DirectX::XMVector2LengthSq(v));

						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p); // ターゲット - プレイヤー
						DirectX::XMStoreFloat(&length1, DirectX::XMVector2LengthSq(v));

						if (length1 < length2)
						{
							lockonEnemy = enemy;
							DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						}
					}
					else // 今回ロックオンしていない
					{
						// 一定範囲外なら返す(Tpsカメラに戻す)
						if (!(PP::Collision::SphereVsSphere(wc.position, (GetRadius() * 30),
							enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT3(0, 0, 0))))
						{
							lockonButtonFlag = false;
							break;
						}
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p);
						DirectX::XMStoreFloat(&length1, DirectX::XMVector3LengthSq(v));

						lockonEnemy = enemy;
						DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						lockonFlag = true;
					}
				}
				//break;
			}
			else // 前回ロックオンしている
			{
				// ロックオンしてた敵の存在が(プレイヤーから一定範囲内)いれば継続
				EnemyManager& enemyManager = EnemyManager::Instance();
				for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
				{
					Enemy* enemy = enemyManager.GetEnemy(i);

					// 一定範囲外なら返す(Tpsカメラに戻す)
					if (!(PP::Collision::SphereVsSphere(wc.position, (GetRadius() * 30),
						enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT3(0, 0, 0))))
					{
						lockonButtonFlag = false;
						break;
					}

					if (enemy == oldLockonEnemy)
					{
						lockonEnemy = enemy;
						lockonFlag = true;
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p);

						//lockonEnemy = enemy;
						DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						break;
					}
					// ロックオンしていた敵がいない場合はTpsカメラに戻す
					else lockonButtonFlag = false;
				}
				// ロックオン対象の切り替え(右スティックX)
				float ax = PP::Input::GetRightStickX();
				lockonTargetChangeTime -= elapsedTime * 60.0f;
				if (lockonEnemy && // ロックオン対象がいて
					lockonTargetChangeTime <= 0 && // 切り替え可能で
					ax * ax > 0.3f) // スティックを倒している
				{
					lockonTargetChangeTime = lockonTargetChangeTimeMax;
					// ロックオン対象と自分の位置からベクトルを算出
					float dx = oldLockonEnemy->GetPosition().x - wc.position.x;
					float dz = oldLockonEnemy->GetPosition().z - wc.position.z;
					float l = sqrtf(dx * dx + dz * dz);
					dx /= l;
					dz /= l;
					// 外積を用いて左右判定を行い、角度的にも最も近い対象をロックオン
					float angleMax = FLT_MAX;
					for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
					{
						Enemy* enemy = enemyManager.GetEnemy(i);
						if (enemy == oldLockonEnemy) continue;
						float dx2 = enemy->GetPosition().x - wc.position.x;
						float dz2 = enemy->GetPosition().z - wc.position.z;
						float l2 = sqrtf(dx2 * dx2 + dz2 * dz2);
						dx2 /= l2;
						dz2 /= l2;
						float cross = dx * dz2 - dz * dx2;
						if (ax > 0 && cross < 0)
						{
							cross = abs(cross);
							if (cross < angleMax)
							{
								angleMax = cross;
								lockonEnemy = enemy;
							}
						}
						else if (ax < 0 && cross > 0)
						{
							if (cross < angleMax)
							{
								angleMax = cross;
								lockonEnemy = enemy;
							}
						}
					}
				}
				//break;
			}
			if (lockonFlag)
			{
				// カメラをロックオンモードにする
				lockonEnemy->SetLockedFlag(true);
				DirectX::XMFLOAT3 enmPos = lockonEnemy->GetPosition();
				float heightOffset = lockonEnemy->GetHeight() * 0.5f;
				Camera::Instance().OnLockonMode(wc.position, enmPos);
				Camera::Instance().SetHeightOffset(heightOffset);
				break;
			}
		}
		// カメラを通常モード(TPS)にする
		Camera::Instance().OnTpsMode(wc.position);
		break;
	}
}

void Player::LockonMoveAnimation()
{
	// ロックオン時の斬後左右移動によるアニメーション設定
	if (lockonFlag)
	{
		DirectX::XMFLOAT3 v = { lockonEnemy->GetPosition().x - wc.position.x,
		0,
		lockonEnemy->GetPosition().z - wc.position.z };
		DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&v); // プレイヤーからエネミーへのベクトル
		DirectX::XMVECTOR M = DirectX::XMLoadFloat3(&(GetMoveVec())); // プレイヤーの移動ベクトル
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(M));
		DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(M));
		float dot = 0.0f;
		DirectX::XMFLOAT3 cross = {};
		DirectX::XMStoreFloat(&dot, Dot);
		DirectX::XMStoreFloat3(&cross, Cross); // c.y = 1 or c.y = -1
		dot = acosf(dot); // 0 ～ 180
		float radian = 0.0f;
		radian = DirectX::XMConvertToRadians(45.0f);

		if (cross.y < 0) dot = (radian * 8) - dot;

		// 右
		if ((dot > (radian * 1) && dot <= (radian * 3)) && runNum != 1) // 右
		{
			runNum = 1;
			TransRunState();
		}
		// 後
		if ((dot > (radian * 3) && dot <= (radian * 5)) && runNum != 2) // 下
		{
			runNum = 2;
			TransRunState();
		}
		// 左
		if ((dot > (radian * 5) && dot <= (radian * 7)) && runNum != 3) // 左
		{
			runNum = 3;
			TransRunState();
		}
		// 前
		if ((dot > (radian * 7) || dot <= (radian * 1)) && runNum != 4) // 上
		{
			runNum = 4;
			TransRunState();
		}
	}
	// 非ロックオン時は前
	if (!lockonFlag && runNum != 4)
	{
		runNum = 4;
		TransRunState();
	}
}

// 待機
void Player::TransIdleState()
{
	state = State::IDLE;

	idleTimer = 600;

	PP::Model::PlayAnimation(model, ANIM_IDLE1, true);
}

void Player::UpdateIdleState(float elapsedTime)
{
	// 入力処理(Walk, Jump, Attack1, Dodge)
	{
		if (InputMove(elapsedTime))
		{
			TransWalkState(); // 歩行ステートへ遷移
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardIdleState();
		}
	}

	// InputProjectile();

	if (guardFlag) idleTimer = 600;
	idleTimer--;

	if (idleTimer == 0) // 一定時間動かない場合別の待機アニメーションを再生
	{
		PP::Model::PlayAnimation(model, PP::System::GetRandF(static_cast<float>(ANIM_IDLE2), static_cast<float>(ANIM_HIT1)), false);
	}

	if (!PP::Model::IsPlayAnimation(model)) // 別のアニメーション再生が終わったらもどる
		TransIdleState();
}

// 歩行
void Player::TransWalkState()
{
	state = State::WALK;

	speed = 2.0f;

	// 前状態でガードしていた場合アニメーション再生しない
	if (!guardedFlag)
		PP::Model::PlayAnimation(model, ANIM_WALK, true);
	else guardedFlag = false;
}

void Player::UpdateWalkState(float elapsedTime)
{
	// 入力処理(Idle, Run, Jump, Attack1, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (!InputWalk())
		{
			TransRunState(); // 走行ステートへ遷移
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardWalkState();
		}
	}

	// InputProjectile();
}

// 走行
void Player::TransRunState()
{
	state = State::RUN;

	speed = 5.0f;

	// 前状態でガードしていた場合アニメーション再生しない
	if (!guardedFlag)
	{
		// 再生する歩行モーションの設定
		switch (runNum)
		{
		case 1: // 右
			//guardOffset = { 20.0f, -60.0f, -50.0f };
			PP::Model::PlayAnimation(model, ANIM_RIGHT_RUN, true);
			break;
		case 2: // 後
			PP::Model::PlayAnimation(model, ANIM_BACK_RUN, true);
			break;
		case 3: // 左
			PP::Model::PlayAnimation(model, ANIM_LFET_RUN, true);
			break;
		case 4: // 前
			PP::Model::PlayAnimation(model, ANIM_RUN1, true);
			break;
		}
	}
	else guardedFlag = false;
		
	//PP::Model::PlayAnimation(model, ANIM_RUN1, true);
}

void Player::UpdateRunState(float elapsedTime)
{
	// 入力処理(Idle, Walk, Dash, Attack1, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (InputWalk())
		{
			TransWalkState(); // 歩行ステートへ遷移
		}

		if (InputDash())
		{
			TransDashState(); // 走行(速い)ステートへ遷移
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardRunState();
		}
	}

	LockonMoveAnimation(); // ロックオンによるアニメーション再生

	// InputProjectile();

}

// 走行(速)
void Player::TransDashState()
{
	state = State::DASH;

	speed = 8.0f;

	// 前状態でガードしていた場合アニメーション再生しない
	if (!guardedFlag)
		PP::Model::PlayAnimation(model, ANIM_RUN2, true);
	else guardedFlag = false;
}

void Player::UpdateDashState(float elapsedTime)
{
	// 入力処理(Idle, Run, Jump, Attack4, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (!InputDash())
		{
			TransRunState(); // 走行ステートへ遷移
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack()) // 加速度でもみて攻撃1か攻撃4への遷移をわける？
		{
			TransAttack4State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}
		
		if (InputGuard())
		{
			TransGuardDashState();
		}
	}

	UseStamina(0.1f, 2.0f);

	// InputProjectile();
}

// 跳躍
void Player::TransJumpState()
{
	state = State::JUMP;

	PP::Model::PlayAnimation(model, ANIM_JUMP, false);
}

void Player::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	//if (InputJump()) // 2段ジャンプは後ほどなくす
	//{
	//	PP::Model::PlayAnimation(model, ANIM_JUMP, false);
	//}

	//if (!PP::Model::IsPlayAnimation(model)) // 条件式を変更?(空中にいてかつ加速度Yがマイナス)
	//{
	//	PP::Model::PlayAnimation(model, ANIM_FALL, true); // 落下状態もつくる
	//}

	//InputProjectile();
}

// 落下
void Player::TransFallState()
{
	state = State::FALL;

	attackCollisionFlag = false;

	PP::Model::PlayAnimation(model, ANIM_FALL, true);
}

void Player::UpdateFallState(float elapsedTime)
{
	fallTimer += elapsedTime;
	InputMove(elapsedTime);
}

// 着地
void Player::TransLandState()
{
	state = State::LAND;

	PP::Model::PlayAnimation(model, ANIM_LANDING1, false);
}

void Player::UpdateLandState(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model) || InputMove(elapsedTime)) // 着地モーションがおわるか移動で状態遷移
	{
		TransIdleState();
	}

	if (InputDodge()) // 着地キャンセル用の回避ステートへ遷移
	{
		TransDodgeState();
	}
}

// 攻撃更新処理
void Player::AttackMoveUpdate(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}

	switch (attackState)
	{
	case 1:
		UpdateAttack1State(elapsedTime);
		break;
	case 2:
		UpdateAttack2State(elapsedTime);
		break;
	case 3:
		UpdateAttack3State(elapsedTime);
		break;
	case 4:
		UpdateAttack4State(elapsedTime);
		break;
	}
}

// 攻撃時の補正
void Player::AttackCorrection(const float correctionTimeStart, const float correctionTimeEnd,
	const float collisionTimeStart, const float collisionTimeEnd, 
	const float advanceTime, float elapsedTime) // ロックオン時は敵の方に向く
{
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	const char* nodeName = "Sword_joint";
	const DirectX::XMFLOAT3 bladePosition = { 13.0f, -3.0f, 50.0f };

	if (animationTime > advanceTime)
	{
		if (InputAttack())
		{
			cancelAttackFlag = true;
			cancelDodgeFlag = false;
		}
		if (InputDodge())
		{
			cancelDodgeFlag = true;
			cancelAttackFlag = false;
		}
	}

	// ロックオンしているかで進行ベクトルの変更
	if (animationTime > correctionTimeStart && animationTime < correctionTimeEnd)
	{
		attackDirection = { GetMoveVec().x, GetMoveVec().z };
		if (attackDirection.x == 0.0f && attackDirection.y == 0.0f) // 動いてない場合は前方ベクトルを設定
			attackDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
		if (lockonFlag)
		{
			attackDirection = { lockonDirection.x, lockonDirection.z };
		}
		DirectX::XMVECTOR AttackDirection = DirectX::XMLoadFloat2(&attackDirection);
		AttackDirection = DirectX::XMVector2Normalize(AttackDirection);
		DirectX::XMStoreFloat2(&attackDirection, AttackDirection);
		const float turnSpeed3 = turnSpeed * 3.0f;
		Move(attackDirection.x, attackDirection.y, speed);
		Turn(attackDirection.x, attackDirection.y, turnSpeed3, elapsedTime);
	}

	attackCollisionFlag = animationTime > collisionTimeStart && animationTime < collisionTimeEnd; // 判定発生時間
	if (attackCollisionFlag)
	{
		weaponPosition = NodeToNewPosition(nodeName, bladePosition); // 指定ノードを基準に判定用の座標を設定
		// 指定ノードとエネミーの衝突処理
		CollisionAttackVsEnemies(weaponPosition, nodeRadius);
	}

	if (animationTime > collisionTimeEnd) // 判定発生時間を超えたらキャンセル可能
	{
		if (cancelAttackFlag)
		{
			if (attackState == 2) TransAttack3State(); // 3段目へ派生
			if (attackState == 1) TransAttack2State(); // 2段目へ派生
		}

		if (cancelDodgeFlag) TransDodgeState(); // 回避
	}

}

// 攻撃1
void Player::TransAttack1State()
{
	state = State::ATTACK;

	attackState = 1;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 4.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK1, false);
}

void Player::UpdateAttack1State(float elapsedTime)
{
	const float correctionTimeStart = 0.25f; // 補正受付時間(始め)
	const float correctionTimeEnd = 0.45f; // 補正受付時間(終わり)
	const float collisionTimeStart = 0.425f; // 判定発生時間(始め)
	const float collisionTimeEnd = 0.525f; // 判定発生時間(終わり)
	const float advanceTime = 0.0f; // とりあえず0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd, 
		advanceTime, elapsedTime);
}

// 攻撃2
void Player::TransAttack2State()
{
	state = State::ATTACK;

	attackState = 2;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 4.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK2, false);
}

void Player::UpdateAttack2State(float elapsedTime)
{
	const float correctionTimeStart = 0.05f; // 補正受付時間(始め)
	const float correctionTimeEnd = 0.2f; // 補正受付時間(終わり)
	const float collisionTimeStart = 0.15f; // 判定発生時間(始め)
	const float collisionTimeEnd = 0.4f; // 判定発生時間(終わり)
	const float advanceTime = 0.0f; // とりあえず0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd, 
		advanceTime, elapsedTime);
}

// 攻撃3
void Player::TransAttack3State()
{
	state = State::ATTACK;

	attackState = 3;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 7.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK3, false);
}

void Player::UpdateAttack3State(float elapsedTime)
{
	const float correctionTimeStart = 0.075f; // 補正受付時間(始め)
	const float correctionTimeEnd = 0.3f; // 補正受付時間(終わり)
	const float collisionTimeStart = 0.25f; // 判定発生時間(始め)
	const float collisionTimeEnd = 0.55f; // 判定発生時間(終わり)
	const float advanceTime = 0.0f; // とりあえず0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd,
		advanceTime, elapsedTime);
}

// 攻撃4(DA)
void Player::TransAttack4State()
{
	state = State::ATTACK;

	attackState = 4;

	cancelAttackFlag = false;

	attackDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
	DirectX::XMVECTOR AttackDirection = DirectX::XMLoadFloat2(&attackDirection);
	AttackDirection = DirectX::XMVector2Normalize(AttackDirection);
	DirectX::XMStoreFloat2(&attackDirection, AttackDirection);
	speed = 7.0f;
	UseStamina(10.0f, 7.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK4, false);
}

void Player::UpdateAttack4State(float elapedTime) // DAは先行入力を付けていない(つけるかも？)
{
	// 任意のアニメーション再生区間でのみ衝突判定処理
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	const float collisionTimeStart = 0.6f; // 判定発生時間(始め)
	const float collisionTimeEnd = 0.8f; // 判定発生時間(終わり)
	const float moveTime = 0.65f;
	const char* nodeName = "Sword_joint";
	const DirectX::XMFLOAT3 bladePosition = { 13.0f, -3.0f, 50.0f };

	attackCollisionFlag = animationTime > collisionTimeStart && animationTime < collisionTimeEnd; // 判定発生時間
	if (attackCollisionFlag)
	{
		weaponPosition = NodeToNewPosition(nodeName, bladePosition); // 指定ノードを基準に判定用の座標を設定
		// 指定ノードとエネミーの衝突処理
		CollisionAttackVsEnemies(weaponPosition, nodeRadius);
	}

	if (animationTime < moveTime) // 跳んでる間攻撃方向に進行
	{
		Move(attackDirection.x, attackDirection.y, speed);
	}

	if (animationTime > collisionTimeEnd) // 判定発生時間を超えたらキャンセル可能
	{
		if (InputDodge()) TransDodgeState();
	}

}

void Player::TransDodgeState()
{
	state = State::DODGE;

	attackCollisionFlag = false;

	cancelDodgeFlag = false;

	// 回避の進行ベクトルを設定
	dodgeDirection = { GetMoveVec().x, GetMoveVec().z };
	if (dodgeDirection.x == 0.0f && dodgeDirection.y == 0.0f) // 動いてない場合は前方ベクトルを設定
		dodgeDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
	DirectX::XMVECTOR DodgeDirection = DirectX::XMLoadFloat2(&dodgeDirection);
	DodgeDirection = DirectX::XMVector2Normalize(DodgeDirection);
	DirectX::XMStoreFloat2(&dodgeDirection, DodgeDirection);

	invincibleTimer = 0.5f;
	speed = 8.0f;
	UseStamina(10.0f, 10.0f);

	PP::Model::PlayAnimation(model, ANIM_DODGE, false);
}

// 回避
void Player::UpdateDodgeState(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}

	if (InputDodge()) cancelDodgeFlag = true;

	// 仮置き処理()
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	if (animationTime > 0.01f && animationTime < 0.8f)
	{
		Move(dodgeDirection.x, dodgeDirection.y, speed);
	}
	else if (animationTime >= 0.8f && animationTime < 1.3f)
	{
		Move(dodgeDirection.x, dodgeDirection.y, speed / 2);
	}

	if (animationTime > 0.7f) // 回避キャンセル回避
	{
		if (cancelDodgeFlag)
		{
			TransDodgeState();
		}
	}

	Turn(dodgeDirection.x, dodgeDirection.y, turnSpeed * 2.0f, elapsedTime);
}

// 被弾
void Player::TransDamageState()
{
	state = State::DAMAGE;

	attackCollisionFlag = false;

	// ダメージモーション再生
	PP::Model::PlayAnimation(model, ANIM_HIT1, false);
}

void Player::UpdateDamageState(float elapsedTime)
{
	// ダメージモーションが終わったら待機ステートへ遷移
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}
}

// 死亡
void Player::TransDeathState()
{
	state = State::DEATH;

	// 死亡アニメーション再生
	PP::Model::PlayAnimation(model, ANIM_DEATH1, false);
}

void Player::TransDeath2State()
{
	state = State::DEATH;

	// 死亡アニメーション再生
	PP::Model::PlayAnimation(model, ANIM_DEATH2, false);
	PP::Model::SetCurrentAnimationSeconds(model, 3.0f);
}

void Player::UpdateDeathState(float elapsedTime)
{
	// 死亡モーションが終わったらOVERへ設定
	if (!PP::Model::IsPlayAnimation(model))
	{
		SceneManager::Instance()->SetChangeType(SceneManager::OVER);
	}
}

// ガード待機
void Player::TransGuardIdleState()
{
	state = State::GUARD_IDLE;

	guardOffset = { 10.0f, 0.0f, 0.0f };

	if (!guardFlag) // 前回の状態でガードをしていないなら
	{
		// ガード状態
		guardFlag = true;
		// 上半身だけガードモーション再生
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardIdleState(float elapsedTime)
{
	// 入力処理
	{
		if (!InputGuard())
		{
			DisGuard(); // ガード解除

			TransIdleState(); // 走行ステートへ遷移
		}

		if (InputMove(elapsedTime))
		{
			TransWalkState(); // 歩行ステートへ遷移
		}

		if (InputAttack())
		{
			DisGuard(); // ガード解除

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // ガード解除

			TransDodgeState();
		}
	}

	UpdateGuard(); // ガード中の更新

}

// ガード歩行
void Player::TransGuardWalkState()
{
	state = State::GUARD_WALK;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // 前回の状態でガードをしていないなら
	{
		// ガード状態
		guardFlag = true;
		// 上半身だけガードモーション再生
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardWalkState(float elapsedTime)
{
	// 入力処理
	{
		if (!InputGuard()) // ガード解除
		{
			guardedFlag = true; // アニメーション管理のためここで立てる

			DisGuard(); // ガード解除

			TransWalkState(); // 走行ステートへ遷移
		}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (!InputWalk())
		{
			TransRunState(); // 走行ステートへ遷移
		}

		if (InputAttack())
		{
			DisGuard(); // ガード解除

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // ガード解除

			TransDodgeState();
		}
	}

	UpdateGuard(); // ガード中の更新

}

// ガード走行
void Player::TransGuardRunState()
{
	state = State::GUARD_RUN;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // 前回の状態でガードをしていないなら
	{
		// ガード状態
		guardFlag = true;
		// 上半身だけガードモーション再生
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardRunState(float elapsedTime)
{
	// 入力処理
	{
		if (!InputGuard()) // ガード解除
		{
			guardedFlag = true; // アニメーション管理のためここで立てる

			DisGuard(); // ガード解除

			TransRunState(); // 走行ステートへ遷移
		}

		//if (/*ガードした*/)
		//{
		//	// GuardHitedステートへ遷移
		//}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (InputWalk())
		{
			TransWalkState(); // 歩行ステートへ遷移
		}

		if (InputDash())
		{
			TransDashState(); // 走行(速い)ステートへ遷移
		}

		if (InputAttack())
		{
			DisGuard(); // ガード解除

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // ガード解除

			TransDodgeState();
		}
	}

	UpdateGuard(); // ガード中の更新

	LockonMoveAnimation(); // ロックオンによるアニメーション再生
}

// ガード走行(速い)
void Player::TransGuardDashState()
{
	state = State::GUARD_DASH;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // 前回の状態でガードをしていないなら
	{
		// ガード状態
		guardFlag = true;
		// 上半身だけガードモーション再生
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardDashState(float elapsedTime)
{
	// 入力処理
	{
		if (!InputGuard()) // ガード解除
		{
			guardedFlag = true; // アニメーション管理のためここで立てる

			DisGuard(); // ガード解除

			TransDashState(); // 走行ステートへ遷移
		}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // 待機ステートへ遷移
		}

		if (!InputDash())
		{
			TransRunState(); // 走行ステートへ遷移
		}

		if (InputAttack()) // 加速度でもみて攻撃1か攻撃4への遷移をわける？
		{
			DisGuard(); // ガード解除

			TransAttack4State();
		}

		if (InputDodge())
		{
			DisGuard(); // ガード解除

			TransDodgeState();
		}
	}

	UpdateGuard(); // ガード中の更新

	UseStamina(0.1f, 2.0f);

}

// ガードリアクション
void Player::TransGuardReactionState()
{
	state = State::GUARD_REACTION;

	PP::Model::PlayAnimation(model, ANIM_GUARD3, false);
}

void Player::UpdateGuardReactionState(float elapsedTime)
{
	// モーションが終わったらガード待機ステートへ遷移
	if (!PP::Model::IsPlayAnimation(model))
	{
		DisGuard(); // ガード解除

		TransIdleState();
	}
}

void Player::UpdateGuard() // ガード中の更新処理
{
	PP::ppNode n = PP::Model::FindNode(model, "LeftShoulder");
	// 指定ノードのアングルを調整
	DirectX::XMVECTOR Rot = DirectX::XMLoadFloat4(&n->rotate);
	DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(guardOffset.x),
		DirectX::XMConvertToRadians(guardOffset.y),
		-DirectX::XMConvertToRadians(guardOffset.z));
	Rot = DirectX::XMQuaternionMultiply(Rot, Q);
	DirectX::XMStoreFloat4(&n->rotate, Rot);
}

void Player::DisGuard() // ガード解除
{
	guardFlag = false;
	PP::Model::StopSubAnimation(model);
}

// プレイヤーとエネミーの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突判定
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突(プレイヤーが推される)
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::CylinderVsCylinder(
			enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
			wc.position, radius, height,
			outPosition))
		{
			this->SetPosition(outPosition);
		}
	}
}

// 自機弾とエネミーの衝突処理
void Player::CollisionProjectileVsEnemies()
{

}

void Player::CollisionAttackVsEnemies(const DirectX::XMFLOAT3& attackPosition, float nodeRadius)
{
	// 指定の座標と全ての敵を総当たりで衝突処理
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		// 衝突
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::SphereVsCylinder(
			attackPosition, nodeRadius,
			enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
			outPosition))
		{
			// 敵にダメージ(0.25f無敵)
			if (enemy->ApplyDamage(1, 0.225f))
			{
				// 攻撃されたフラグ
				enemy->SetAttackedFlag(true);

				// 吹っ飛ばす(たぶんなくなる)
				{
					/*const float power = 10.0f;
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
					float vx = e.x - attackPosition.x;
					float vz = e.z - attackPosition.z;
					float lengthXZ = sqrtf((vx * vx) + (vz * vz));
					vx /= lengthXZ;
					vz /= lengthXZ;
					DirectX::XMFLOAT3 impulse = {};
					impulse.x = vx * power;
					impulse.y = power * 0.5f;
					impulse.z = vz * power;*/

					//enemy->AddImpulse(impulse);
				}
			}
		}
	}

}

DirectX::XMFLOAT3 Player::NodeToNewPosition(const char* nodeName, const DirectX::XMFLOAT3& position)
{
	PP::ppNode node = PP::Model::FindNode(model, nodeName);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T; // 仮のワールド行列を作成
	DirectX::XMMATRIX BoneWorldTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);

	DirectX::XMFLOAT4X4 w;
	DirectX::XMStoreFloat4x4(&w, (W * BoneWorldTransform));

	return { w._41, w._42, w._43 };
}