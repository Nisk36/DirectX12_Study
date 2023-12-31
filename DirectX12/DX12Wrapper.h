#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<map>
#include<memory>
#include<unordered_map>
#include<DirectXTex.h>
#include<wrl.h>
#include<string>
#include<functional>

class Dx12Wrapper
{
	SIZE _winSize;
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//DXGIまわり
	ComPtr < IDXGIFactory4> _dxgiFactory = nullptr;//DXGIインターフェイス
	ComPtr < IDXGISwapChain4> _swapchain = nullptr;//スワップチェイン

	//DirectX12まわり
	ComPtr< ID3D12Device> _dev = nullptr;//デバイス
	ComPtr < ID3D12CommandAllocator> _cmdAllocator = nullptr;//コマンドアロケータ
	ComPtr < ID3D12GraphicsCommandList> _cmdList = nullptr;//コマンドリスト
	ComPtr < ID3D12CommandQueue> _cmdQueue = nullptr;//コマンドキュー

	//表示に関わるバッファ周り
	ComPtr<ID3D12Resource> _depthBuffer = nullptr;//深度バッファ
	std::vector<ID3D12Resource*> _backBuffers;//バックバッファ(2つ以上…スワップチェインが確保)
	ComPtr<ID3D12DescriptorHeap> _rtvHeaps = nullptr;//レンダーターゲット用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;//深度バッファビュー用デスクリプタヒープ
	std::unique_ptr<D3D12_VIEWPORT> _viewport;//ビューポート
	std::unique_ptr<D3D12_RECT> _scissorrect;//シザー矩形

	//シーンを構成するバッファまわり
	ComPtr<ID3D12Resource> _sceneConstBuff = nullptr;

	struct SceneData {
		DirectX::XMMATRIX view;//ビュー行列
		DirectX::XMMATRIX proj;//プロジェクション行列
		DirectX::XMFLOAT3 eye;//視点座標
	};
	SceneData* _mappedSceneData;
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

	//フェンス
	ComPtr<ID3D12Fence> _fence = nullptr;
	UINT64 _fenceVal = 0;

	//最終的なレンダーターゲットの生成
	HRESULT	CreateFinalRenderTargets();
	//デプスステンシルビューの生成
	HRESULT CreateDepthStencilView();

	//1枚目レンダリング用
	//いわゆるペラポリに張り付けるための絵の
	//メモリリソースとそのビュー
	ComPtr<ID3D12DescriptorHeap> _peraRTVHeap;
	ComPtr<ID3D12DescriptorHeap> _peraRegisterHeap;
	ComPtr<ID3D12Resource> _peraResource;
	//１枚目ペラポリのためのリソースとビューを
	//作成
	bool CreatePeraResourcesAndView();

	//スワップチェインの生成
	HRESULT CreateSwapChain(const HWND& hwnd);

	//DXGIまわり初期化
	HRESULT InitializeDXGIDevice();



	//コマンドまわり初期化
	HRESULT InitializeCommand();

	//ビュープロジェクション用ビューの生成
	HRESULT CreateSceneView();

	//ロード用テーブル
	using LoadLambda_t = std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::map < std::string, LoadLambda_t> _loadLambdaTable;
	//テクスチャテーブル
	std::unordered_map<std::string, ComPtr<ID3D12Resource>> _textureTable;
	//テクスチャローダテーブルの作成
	void CreateTextureLoaderTable();
	//テクスチャ名からテクスチャバッファ作成、中身をコピー
	ID3D12Resource* CreateTextureFromFile(const char* texpath);

	//ペラポリ用頂点バッファ(N字の4点)
	ComPtr<ID3D12Resource> _peraVB;
	D3D12_VERTEX_BUFFER_VIEW _peraVBV;

	//ペラポリ用パイプライン＆ルートシグネチャ
	ComPtr<ID3D12PipelineState> _peraPipeline;
	ComPtr<ID3D12RootSignature> _peraRS;

	bool CreatePeraVertex();
	bool CreatePeraPipeline();



public:
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	void Update();
	void BeginDraw();
	void Draw();
	void EndDraw();
	void ExecuteCommand();
	///テクスチャパスから必要なテクスチャバッファへのポインタを返す
	///@param texpath テクスチャファイルパス
	ComPtr<ID3D12Resource> GetTextureByPath(const char* texpath);

	ComPtr< ID3D12Device> Device();//デバイス
	ComPtr < ID3D12GraphicsCommandList> CommandList();//コマンドリスト
	ComPtr < IDXGISwapChain4> Swapchain();//スワップチェイン

	void SetScene();

	//ペラポリゴンへの描画準備
	bool PreDrawToPera1();
	//ペラポリゴンへの描画後処理
	void PostDrawToPera1();


};


