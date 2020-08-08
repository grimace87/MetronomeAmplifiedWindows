#pragma once

#include "Common\DeviceResources.h"
#include "MetronomeAmplifiedWindowsMain.h"

#include <winrt/Windows.ApplicationModel.Activation.h>

namespace MetronomeAmplifiedWindows
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
	{
	public:
		App();

		// IFrameworkViewSource methods
		winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView();

		// IFrameworkView methods
		virtual void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
		virtual void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
		virtual void Load(winrt::hstring const& entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	protected:
		// Application lifecycle event handlers.
		void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
		void OnSuspending(IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
		void OnResuming(IInspectable const& sender, IInspectable const& args);

		// Window event handlers.
		void OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args);
		void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);
		void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args);
		void OnPointerPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);

		// DisplayInformation event handlers.
		void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args);
		void OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args);
		void OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args);

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<MetronomeAmplifiedWindowsMain> m_main;
		bool m_windowClosed;
		bool m_windowVisible;
	};
}

class Direct3DApplicationSource : winrt::Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	
};
