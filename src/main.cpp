/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/binding/LoadingCircle.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

/**
 * `$modify` lets you extend and modify GD's classes.
 * To hook a function in Geode, simply $modify the class
 * and write a new function definition with the signature of
 * the function you want to hook.
 *
 * Here we use the overloaded `$modify` macro to set our own class name,
 * so that we can use it for button callbacks.
 *
 * Notice the header being included, you *must* include the header for
 * the class you are modifying, or you will get a compile error.
 *
 * Another way you could do this is like this:
 *
 * struct MyLoadingLayer : Modify<MyLoadingLayer, LoadingLayer> {};
 */
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/cocos/CCDirector.h>
class $modify(SplitUpdateLayer, LoadingLayer)
{
	struct Fields
	{
		async::TaskHolder<web::WebResponse> m_listener;
	}
	/**
	 * Typically classes in GD are initialized using the `init` function, (though not always!),
	 * so here we use it to add our own button to the bottom menu.
	 *
	 * Note that for all hooks, your signature has to *match exactly*,
	 * `void init()` would not place a hook!
	 */
	void
	loadingFinished()
	{
		auto version = "v1.0.0";
		/**
		 * We call the original init function so that the
		 * original class is properly initialized.
		 */

		/**
		 * You can use methods from the `geode::log` namespace to log messages to the console,
		 * being useful for debugging and such. See this page for more info about logging:
		 * https://docs.geode-sdk.org/tutorials/logging
		 */

		/**
		 * See this page for more info about buttons
		 * https://docs.geode-sdk.org/tutorials/buttons
		 */
		this.m_textArea->setString("Checking for SplitGDPS updates...");
		this->updateProgress(0);
		web::WebRequest req = web::WebRequest();
		req.param("version", version);
		req.userAgent("");
		req.timeout(std::chrono::seconds(30));
		auto ls = LoadingCircle::create();
		ls->show();
		auto future = req.get("https://split.ps.fhgdps.com/getSGNeedUpdate.php");
		this->m_fields->m_listener.spawn(
			"check update",
			std::move(future), // you can also do `req.post()` directly here instead
			[](web::WebResponse response)
			{
				auto res = response.json();
				// matjson::Value thing is returned from res
				//  Get the value of "latest" key
				auto latest = res["latest"];
				auto current = version;
				auto updateURL = res["updateURL"];
				auto forceUpdate = res["forceUpdate"];
				loadingCircle->fadeAndRemove();
				this->updateProgress(10);
				if (latest != current)
				{
					if (forceUpdate)
					{
						geode::createQuickPopup("Update Requred", "A required update to SplitGDPS is available.\nVersion: " + latest.asString() + "\nCurrentVersion: " + current.asString(), "exit", "Update", [](auto bool btn2)
												{
							if (btn2)
							{
								this->updateProgress(15);
								this.m_textArea->setString("Updating SplitGDPS");
								// TODO: actually make this
							} else
							{
								CCDirector::sharedDirector()->end();
							} });
					}
					else
					{
						geode::createQuickPopup("Update Available", "An update to SplitGDPS is available.\nVersion: " + latest.asString() + "\nCurrentVersion: " + current.asString(), "Later", "Update", [](auto bool btn2)
												{
							if (btn2)
							{
								this->updateProgress(15);
								this.m_textArea->setString("Updating SplitGDPS");
							} else {
								LoadingLayer::loadingFinished();
								return;
							} });
					}
				}
			});

		/**
		 * We return `true` to indicate that the class was properly initialized.
		 */

		LoadingLayer::loadingFinished()
	}
