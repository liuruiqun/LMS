<?php

class WSNtableController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';

	/**
	 * @return array action filters
	 */
	public function filters()
	{
		return array(
			'accessControl', // perform access control for CRUD operations
			'postOnly + delete', // we only allow deletion via POST request
		);
	}

	/**
	 * Specifies the access control rules.
	 * This method is used by the 'accessControl' filter.
	 * @return array access control rules
	 */
	public function accessRules()
	{
		return array(
			array('allow',  // allow all users to perform 'index' and 'view' actions
				'actions'=>array('index','view','rainfall','porepressure','stage','stage_113','stage_203','soilmoisture',
                    'soilmoisture_302','soilmoisture_306','chartrainfall','Chartstage_113','vibration'),
				'users'=>array('*'),
			),
			array('allow', // allow authenticated user to perform 'create' and 'update' actions
				'actions'=>array('create','update'),
				'users'=>array('@'),
			),
			array('allow', // allow admin user to perform 'admin' and 'delete' actions
				'actions'=>array('admin','delete'),
				'users'=>array('admin'),
			),
			array('deny',  // deny all users
				'users'=>array('*'),
			),
		);
	}

	/**
	 * Displays a particular model.
	 * @param integer $id the ID of the model to be displayed
	 */
	public function actionView($id)
	{
		$this->render('view',array(
			'model'=>$this->loadModel($id),
		));
	}

	/**
	 * Creates a new model.
	 * If creation is successful, the browser will be redirected to the 'view' page.
	 */
	public function actionCreate()
	{
		$model=new WSNtable;

		// Uncomment the following line if AJAX validation is needed
		// $this->performAjaxValidation($model);

		if(isset($_POST['WSNtable']))
		{
			$model->attributes=$_POST['WSNtable'];
			if($model->save())
				$this->redirect(array('view','id'=>$model->NUMSEQ));
		}

		$this->render('create',array(
			'model'=>$model,
		));
	}

	/**
	 * Updates a particular model.
	 * If update is successful, the browser will be redirected to the 'view' page.
	 * @param integer $id the ID of the model to be updated
	 */
	public function actionUpdate($id)
	{
		$model=$this->loadModel($id);

		// Uncomment the following line if AJAX validation is needed
		// $this->performAjaxValidation($model);

		if(isset($_POST['WSNtable']))
		{
			$model->attributes=$_POST['WSNtable'];
			if($model->save())
				$this->redirect(array('view','id'=>$model->NUMSEQ));
		}

		$this->render('update',array(
			'model'=>$model,
		));
	}

	/**
	 * Deletes a particular model.
	 * If deletion is successful, the browser will be redirected to the 'admin' page.
	 * @param integer $id the ID of the model to be deleted
	 */
	public function actionDelete($id)
	{
		$this->loadModel($id)->delete();

		// if AJAX request (triggered by deletion via admin grid view), we should not redirect the browser
		if(!isset($_GET['ajax']))
			$this->redirect(isset($_POST['returnUrl']) ? $_POST['returnUrl'] : array('admin'));
	}

	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		$dataProvider=new CActiveDataProvider('WSNtable');
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
		));
	}



	/**
	 * Manages all models.
	 */
	public function actionAdmin()
	{
		$model=new WSNtable('search');
		$model->unsetAttributes();  // clear any default values
		if(isset($_GET['WSNtable']))
			$model->attributes=$_GET['WSNtable'];

		$this->render('admin',array(
			'model'=>$model,
		));
	}

    /**
     * 雨量、土壤湿度、孔压、水位等数据呈现及绘图
     **/

    //Rainfall
    public function  actionRainfall()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "NODEID = :node",
                'params'    => array(':node' => '0x201'),
            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('rainfall',array(
            'dataProvider'=>$dataProvider,
        ));
    }


    protected  function getMonthRainCount($month) {
        $criteria = new CDbCriteria; // 创建CDbCriteria对象
        $criteria->addCondition("NODEID = :keyword");
        $criteria->params[':keyword'] = '0x201';
        $criteria->addBetweenCondition('DatePart(yy,RECVTIME)', 2014,2014);
        $criteria->addBetweenCondition('DatePart(mm,RECVTIME)', $month,$month);
        $criteria->select = 'VALUE';
        $criteria->order = 'NUMSEQ desc';
        $criteria -> limit = 3;
        $rainfall = WSNtable::model()->findAll($criteria);
        $count = 0;
        foreach ($rainfall as $item) {
            $count += $item->VALUE;
        }
        $count *=  0.5;
        return $count * 0.5;//:mm
    }

    protected function getMonthRainCountsInBetween($startMonth,$endMonth) {
        $rlt = array();
        for($i=$startMonth;$i<=$endMonth;$i++) {
            $rlt[$i] = self::getMonthRainCount($i);
        }
        return $rlt;
        //return array(1=>12,2=>0.... 5=>22);
    }

    /*public function  actionChartrainfall()
    {
        $dataProvider = self::getMonthRainCountsInBetween(1,12);
        $this->render('chartrainfall',array(
            'dataProvider'=>$dataProvider,
        ));
    }*/

    public function  actionChartrainfall()
    {
        $dataProvider = array(178, 75, 33, 13, 15, 23, 41);
        $this->render('chartrainfall',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    //Stage
    public function  actionStage()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "TYPENOTE = :keyword",
                'params'    => array(':keyword' =>'水位'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('stage',array(
            'dataProvider'=>$dataProvider,
        ));
    }
    //node 113
    public function  actionStage_113()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "NODEID = :node",
                'params'    => array( ':node' => '0x113'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('stage_113',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    protected  function getMonthStageCount($month) {
        $criteria = new CDbCriteria; // 创建CDbCriteria对象
        $criteria->addCondition("NODEID = :keyword");
        $criteria->params[':keyword'] = '0x113';
        $criteria->addBetweenCondition('DatePart(yy,RECVTIME)', 2014,2014);
        $criteria->addBetweenCondition('DatePart(mm,RECVTIME)', $month,$month);
        $criteria->select = 'VALUE';
        $criteria->order = 'NUMSEQ desc';
        $criteria -> limit = 3;
        $rainfall = WSNtable::model()->findAll($criteria);
        $count = 0;
        $i = 0;
        foreach ($rainfall as $item) {
            $count += $item->VALUE;
            $i ++;
        }
        $count = 7000-(30000 - $count / $i);
        //$count = 3000 - intval($count);
        return $count;
    }

    protected function getMonthStageCountsInBetween($startMonth,$endMonth) {
        $rlt = array();
        for($i=$startMonth;$i<=$endMonth;$i++) {
            $rlt[$i] = self::getMonthStageCount($i);
        }
        return $rlt;

    }


    /*public function  actionChartstage_113()
    {
        $dataProvider = self::getMonthStageCountsInBetween(1,12);
        $this->render('chartstage_113',array(
            'dataProvider'=>$dataProvider,
        ));
    }*/

    public function  actionChartstage_113()
    {
        $dataProvider = array(270, 120, 61, 30, 31, 11,22);
        $this->render('chartstage_113',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionStage_203()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "NODEID = :node",
                'params'    => array( ':node' => '0x203'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('stage_203',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionPorepressure()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "NODEID = :node",
                'params'    => array(':node' => '0x305'),
            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('porepressure',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionSoilmoisture()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "TYPENOTE = :keyword",
                'params'    => array(':keyword' =>'土壤湿度'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('soilmoisture',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionSoilmoisture_302()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "TYPENOTE = :type and NODEID = :node",
                'params'    => array(':type' => '土壤湿度', ':node' => '0x302'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('soilmoisture_302',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionSoilmoisture_306()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "TYPENOTE = :type and NODEID = :node",
                'params'    => array(':type' => '土壤湿度', ':node' => '0x306'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('soilmoisture_306',array(
            'dataProvider'=>$dataProvider,
        ));
    }

    public function  actionVibration ()
    {
        $dataProvider =new CActiveDataProvider('WSNtable',
            array('criteria' => array('select'    => "*",
                'order' => "NUMSEQ desc",
                'condition'=> "NODEID = :node",
                'params'    => array( ':node' => '0x304'),

            ),
                'pagination'=>array(
                    'pageSize'=>20,
                ),

            ));
        $this->render('vibration',array(
            'dataProvider'=>$dataProvider,
        ));
    }

	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer $id the ID of the model to be loaded
	 * @return WSNtable the loaded model
	 * @throws CHttpException
	 */
	public function loadModel($id)
	{
		$model=WSNtable::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}

	/**
	 * Performs the AJAX validation.
	 * @param WSNtable $model the model to be validated
	 */
	protected function performAjaxValidation($model)
	{
		if(isset($_POST['ajax']) && $_POST['ajax']==='wsntable-form')
		{
			echo CActiveForm::validate($model);
			Yii::app()->end();
		}
	}
}
