/*

class ObjectUtils {
private: 

	struct ObjectBoundaries {
		float t, b, l, r; // top, bottom, left, right
	};

	// fix for the GameObject setPosition function
	static void setGameObjScalePositionFix(GameObject* obj, CCPoint pos, float scaleMultiplier = 1) {
		if (obj->m_colorSprite) {
			auto sprPos = obj->m_colorSprite->getPosition();
			auto sprSc = obj->m_colorSprite->getScale();
			obj->setPosition(pos);
			obj->setScale(obj->getScale() * scaleMultiplier);
			obj->m_colorSprite->setPosition(sprPos);
			obj->m_colorSprite->setScale(sprSc);
		} else {
			obj->setPosition(pos);
			obj->setScale(obj->getScale() * scaleMultiplier);
		}
	}


	static CCPoint convertToBaseNode(CCNode* baseNode, CCNode* targetNode, CCPoint targetNodePoint) {
		while (targetNode != baseNode) {
			targetNodePoint = CCPointApplyAffineTransform(targetNodePoint, targetNode->nodeToParentTransform());
			targetNode = targetNode->getParent();
		}
		return targetNodePoint;
	}


	static void collectAllchildren(CCArray* array, CCNode* obj) {
		for (auto* child : CCArrayExt<CCNode*>(obj->getChildren())) {
			collectAllchildren(array, child);
		}
		array->addObject(obj);
	}


	static ObjectBoundaries recursiveGetRealSize(CCNode* obj, CCNode* objParent) { 
		float t = -9999, b = 9999, l = 9999, r = -9999;
		auto nodes = CCArray::create();
		collectAllchildren(nodes, obj);

		for (auto* node : CCArrayExt<CCNode*>(nodes)) {
			auto box = node->boundingBox(); // in parent coords
			auto tr = convertToBaseNode(objParent, node->getParent(), ccp(box.getMaxX(), box.getMaxY()));
			auto bl = convertToBaseNode(objParent, node->getParent(), ccp(box.getMinX(), box.getMinY()));
			if (tr.x > r) r = tr.x;
			if (bl.x < l) l = bl.x;
			if (tr.y > t) t = tr.y;
			if (bl.y < b) b = bl.y;
		}
		return {t, b, l, r};
	}

public:
	//  ccc4(0, 255, 0, 150) : ccc4(255, 0, 0, 150)
	static void debugDrawBoundaries(CreateMenuItem* cmi, ccColor4B color) {
		auto col = CCLayerColor::create(color);
		col->ignoreAnchorPointForPosition(false);

		auto btnSpr = static_cast<ButtonSprite*>(cmi->getNormalImage());
		auto obj = static_cast<GameObject*>(btnSpr->m_subSprite);
		auto size = recursiveGetRealSize(obj, btnSpr);

		col->setContentSize({size.r - size.l, size.t - size.b});
		btnSpr->addChild(col, 10);
		col->setPosition(obj->getPosition());
	}

	
	static void scaleObject(GameObject* obj, float scaleX, float scaleY) {
		if (obj->m_colorSprite) {
			auto sprPos = obj->m_colorSprite->getPosition();
			auto sprScX = obj->m_colorSprite->getScaleX();
			auto sprScY = obj->m_colorSprite->getScaleY();
			obj->setScaleX(scaleX);
			obj->setScaleY(scaleY);
			obj->m_colorSprite->setPosition(sprPos);
			obj->m_colorSprite->setScaleX(sprScX);
			obj->m_colorSprite->setScaleY(sprScY);
		} else {
			obj->setScaleX(scaleX);
			obj->setScaleY(scaleY);
		}
	}


	static void inline scaleObject(GameObject* obj, float scale) {
		scaleObject(obj, scale, scale);
	}


	static void setObjectPos(GameObject* obj, CCPoint pos) {
		auto deltaPos = pos - ccp(20,22);
		auto childSize = obj->getScaledContentSize();
		auto oldAnchorPos = childSize / 2;
		auto newAnchorPos = oldAnchorPos - deltaPos;
		// old -> (0.5, 0.5), new -> x;
		obj->setAnchorPoint(ccp(newAnchorPos.width / oldAnchorPos.width, newAnchorPos.height / oldAnchorPos.height) * 0.5);
	}


	static void scaleAutoFix(GameObject* obj, CCNode* base) {
		// standard object size is 30x30
		auto bounds = recursiveGetRealSize(obj, base);
		float maxSz = std::max(bounds.r - bounds.l, bounds.t - bounds.b);
		float scale = 30.f / maxSz * obj->getScale();
		scaleObject(obj, scale, scale);
	}


	static void setFrame(EnhancedGameObject* obj, int frame) {
		obj->updateSyncedAnimation(0, frame);
	}


};




class $modify(MyEditorUI, EditorUI) {


	bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		auto lab = CCLabelBMFont::create("str", "bigFont.fnt");
		addChild(lab);
		lab->setPosition({200, 200});
		lab->setID("qwerty"); // todo: delete this

		return true;
	}


	CreateMenuItem* getCreateBtn(int id, int bg) {
		auto btn = EditorUI::getCreateBtn(id, bg);
		auto btnSpr = static_cast<ButtonSprite*>(btn->getNormalImage());
		auto obj = static_cast<GameObject*>(btnSpr->m_subSprite);

		// capture initial state
		float scX0 = obj->getScaleX(), scY0 = obj->getScaleY();
		CCPoint anc0 = obj->getAnchorPoint();


		static const std::unordered_set<short> autoFixObjects = {
			3153, 3154, 3155, 3156, 3157, 3158, 3159, 3160, 
		};

		static const std::unordered_set<short> scaleXYObjects = {
			
		};

		static const std::unordered_set<short> scaleXObjects = {
			
		};

		static const std::unordered_set<short> scaleYObjects = {
			
		};
		
		
		// SCALE
		if (autoFixObjects.contains(id)) {
			ObjectUtils::scaleAutoFix(obj, btnSpr);
			ObjectUtils::debugDrawBoundaries(btn, ccc4(0,255,0,150));

		} else if (scaleXYObjects.contains(id)) {
			ObjectUtils::scaleObject(obj, 30);
			ObjectUtils::debugDrawBoundaries(btn, ccc4(0,0,255,150));

		} else if (scaleXObjects.contains(id)) {
			ObjectUtils::scaleObject(obj, 30, obj->getScale());
			ObjectUtils::debugDrawBoundaries(btn, ccc4(0,0,255,150));

		} else if (scaleYObjects.contains(id)) {
			ObjectUtils::scaleObject(obj, obj->getScale(), 30);
			ObjectUtils::debugDrawBoundaries(btn, ccc4(0,0,255,150));

		}

		// todo: change NOT POSITION BUT ANCHOR POINT (def pos = {20, 22})
		
		
		switch (id) { // 21, 19
			case 4380: // top
			case 2488:
			case 2489:
			case 2490:
			case 2491:
			case 2502:
			case 2503:
			case 4381:
			case 4382:
			case 4383:
			case 2541:
			case 2543:
			case 2545:
			case 2547:
			case 2570:
			case 2572:
			case 2574:
			case 2576:
			case 2578:
			case 2621:
			case 2555:
			case 2557:
			case 2571:
			case 2573:
			case 2575:
			case 2577:
			case 2579:
			case 2638:
			case 2652:
			case 2655:
			case 2682:
			case 2685:
			case 3180:
			case 3353:
			case 3355:
			case 3398:
			case 3399:
			case 3400:
			case 3401:
			case 4010:
			case 4011:
			case 2455:
			case 2440:
			case 4038:
			case 4039:
			case 2661:
			case 2675:
			case 2665:
			case 2680:
			case 3106:
			case 1525:
			case 1526:
			case 1527:
			case 1528:
			case 4368:
			case 4369:
			case 4370:
			{
				ObjectUtils::setObjectPos(obj, ccp(20,16)); 
				ObjectUtils::debugDrawBoundaries(btn, ccc4(255,0,0,30));
				break;
			}

			case 2639: // left
			case 2643:
			case 2645:
			case 2650:
			case 2653:
			case 2656:
			case 2659:
			case 2683:
			case 2686:
			case 3335:
			case 4164:
			{
				ObjectUtils::setObjectPos(obj, ccp(22,22)); 
				ObjectUtils::debugDrawBoundaries(btn, ccc4(255,0,0,30));
				break;
			}

			case 3177: // top left
			case 1194: 
			{
				ObjectUtils::setObjectPos(obj, ccp(21,19)); 
				ObjectUtils::debugDrawBoundaries(btn, ccc4(255,0,0,30));
				break;
			}


			default: break;
		}


		// capture and cache all made changes
		float scX1 = obj->getScaleX(), scY1 = obj->getScaleY();
		CCPoint anc1 = obj->getAnchorPoint();

		if (scX0 != scX1 || scY0 != scY1 || anc0 != anc1) {
			log::debug("{} {}, {} {}, {}, {}, {} {},", 
				"{", obj->m_objectID, "{", scX1, scY1, anc1.x, anc1.y, "}}"
			);
		}

		// todo: check all at the lower textures

		return btn;
	}


	void onCreateButton(CCObject* sender) {
		EditorUI::onCreateButton(sender);

		auto cmi = as<CreateMenuItem*>(sender); // todo: delete this
		as<CCLabelBMFont*>(EditorUI::get()->getChildByID("qwerty"))->setString(fmt::format("obj: {}", cmi->m_objectID).c_str());
	}

};



/* 


void updateAnimatedObject(float delta) {
	struct FrameInfo {
		short total;
		short stopFrame;
	};


	static const std::unordered_map<short, FrameInfo> frameForId = {
		{921, {12, 4}}
	};

	if (auto obj = m_fields->animatedObject) {
		auto it = frameForId.find(obj->m_objectID);
		if (it != frameForId.end()) {
			auto frameInfo = it->second;
			obj->m_singleFrame = (1 + obj->m_singleFrame) % frameInfo.total;
			obj->updateSyncedAnimation(0, obj->m_singleFrame);
		}
	}

}

*/