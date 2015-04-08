<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */

$this->breadcrumbs=array(
	'Wsnkongyadatas'=>array('index'),
	'Manage',
);

$this->menu=array(
	array('label'=>'List WSNkongyadata', 'url'=>array('index')),
	array('label'=>'Create WSNkongyadata', 'url'=>array('create')),
);

Yii::app()->clientScript->registerScript('search', "
$('.search-button').click(function(){
	$('.search-form').toggle();
	return false;
});
$('.search-form form').submit(function(){
	$('#wsnkongyadata-grid').yiiGridView('update', {
		data: $(this).serialize()
	});
	return false;
});
");
?>

<h1>Manage Wsnkongyadatas</h1>

<p>
You may optionally enter a comparison operator (<b>&lt;</b>, <b>&lt;=</b>, <b>&gt;</b>, <b>&gt;=</b>, <b>&lt;&gt;</b>
or <b>=</b>) at the beginning of each of your search values to specify how the comparison should be done.
</p>

<?php echo CHtml::link('Advanced Search','#',array('class'=>'search-button')); ?>
<div class="search-form" style="display:none">
<?php $this->renderPartial('_search',array(
	'model'=>$model,
)); ?>
</div><!-- search-form -->

<?php $this->widget('zii.widgets.grid.CGridView', array(
	'id'=>'wsnkongyadata-grid',
	'dataProvider'=>$model->search(),
	'filter'=>$model,
	'columns'=>array(
		'DID',
		'SID',
		'DataTime',
		'S1',
		'S2',
		'R1',
		/*
		'R2',
		'IsWarning',
		*/
		array(
			'class'=>'CButtonColumn',
		),
	),
)); ?>
