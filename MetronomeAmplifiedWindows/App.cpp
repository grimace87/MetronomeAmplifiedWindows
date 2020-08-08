#include "pch.h"
#include "App.h"

// The main function is only used to initialize our IFrameworkView class.
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	winrt::init_apartment();
	winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<MetronomeAmplifiedWindows::App>());
}

winrt::Windows::ApplicationModel::Core::IFrameworkView MetronomeAmplifiedWindows::App::CreateView()
{
	return *this;
}

MetronomeAmplifiedWindows::App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void MetronomeAmplifiedWindows::App::Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView.Activated({ this, &App::OnActivated });

	winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &App::OnSuspending });

	winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &App::OnResuming });

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
}

// Called when the CoreWindow object is created (or re-created).
void MetronomeAmplifiedWindows::App::SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
{
	window.SizeChanged({ this, &App::OnWindowSizeChanged });
	window.VisibilityChanged({ this, &App::OnVisibilityChanged });
	window.Closed({ this, &App::OnWindowClosed });
	window.PointerPressed({ this, &App::OnPointerPressed });

	winrt::Windows::Graphics::Display::DisplayInformation currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
	currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });
	currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });
	winrt::Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

	m_deviceResources->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void MetronomeAmplifiedWindows::App::Load(winrt::hstring const& entryPoint)
{
	if (m_main == nullptr) {
		m_main = std::make_unique<MetronomeAmplifiedWindowsMain>(m_deviceResources);
		m_main->CreateDeviceDependentResources();
		m_main->CreateWindowSizeDependentResources();
	}
}

// This method is called after the window becomes active.
void MetronomeAmplifiedWindows::App::Run()
{
	while (!m_windowClosed) {
		if (m_windowVisible) {
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
			m_main->Update();
			if (m_main->Render()) {
				m_deviceResources->Present();
			}
		} else {
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void MetronomeAmplifiedWindows::App::Uninitialize()
{
}

// Application lifecycle event handlers.

void MetronomeAmplifiedWindows::App::OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
{
	// Run() won't start until the CoreWindow is activated.
	winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Activate();
}

void MetronomeAmplifiedWindows::App::OnSuspending(IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	winrt::Windows::ApplicationModel::SuspendingDeferral const& deferral = args.SuspendingOperation().GetDeferral();

	Concurrency::create_task([this, deferral]()
	{
        m_deviceResources->Trim();

		// Insert your code here.

		deferral.Complete();
	});
}

void MetronomeAmplifiedWindows::App::OnResuming(IInspectable const& sender, IInspectable const& args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void MetronomeAmplifiedWindows::App::OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args)
{
	m_deviceResources->SetLogicalSize(winrt::Windows::Foundation::Size(sender.Bounds().Width, sender.Bounds().Height));
	m_main->CreateWindowSizeDependentResources();
}

void MetronomeAmplifiedWindows::App::OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
	m_windowVisible = args.Visible();
}

void MetronomeAmplifiedWindows::App::OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args)
{
	m_windowClosed = true;
}

// DisplayInformation event handlers.

void MetronomeAmplifiedWindows::App::OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args)
{
	// Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
	// if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
	// you should always retrieve it using the GetDpi method.
	// See DeviceResources.cpp for more details.
	m_deviceResources->SetDpi(sender.LogicalDpi());
	m_main->CreateWindowSizeDependentResources();
}

void MetronomeAmplifiedWindows::App::OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args)
{
	m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
	m_main->CreateWindowSizeDependentResources();
}

void MetronomeAmplifiedWindows::App::OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, IInspectable const& args)
{
	m_deviceResources->ValidateDevice();
}

void MetronomeAmplifiedWindows::App::OnPointerPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args)
{
	//uint32_t pointerId = args.CurrentPoint().PointerId();
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args.CurrentPoint().Position().X, args.CurrentPoint().Position().Y);
	winrt::Windows::Foundation::Size size = m_deviceResources->GetOutputSize();
	float normalisedX = 2.0f * position.x / size.Width - 1.0f;
	float normalisedY = 1.0f - (2.0f * position.y / size.Height - 1.0f);
	m_main->OnPointerPressed(normalisedX, normalisedY);
}
